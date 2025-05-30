#include "DeviceSettings.h"

DeviceSettings::DeviceSettings(juce::ValueTree tree, std::function<void(const CommandMessage&)> onSettingModified_) : onSettingModified(onSettingModified_), settingsTree(tree.getChildWithName(DeviceSettingsIds::Settings)), rootItem(settingsTree, settingsFlattened, tree.getChildWithName(DeviceSettingsIds::Enums), hideUnusedSettings)
{
    setRootItem(&rootItem);
    setRootItemVisible(false);

    settingsTree.addListener(this);
}

std::vector<CommandMessage> DeviceSettings::getReadCommands() const
{
    std::vector<CommandMessage> commands;
    for (auto setting : settingsFlattened)
    {
        commands.push_back({ setting[DeviceSettingsIds::key], {} });
    }
    return commands;
}

std::vector<CommandMessage> DeviceSettings::getWriteCommands(const bool replaceReadOnlyValuesWithNull) const
{
    std::vector<CommandMessage> commands;
    for (auto setting : settingsFlattened)
    {
        if (setting.hasProperty(DeviceSettingsIds::value) == false)
        {
            continue;
        }

        if (setting[DeviceSettingsIds::readOnly] && replaceReadOnlyValuesWithNull)
        {
            commands.push_back({ setting[DeviceSettingsIds::key], {} });
            continue;
        }

        commands.push_back(getWriteCommand(setting));
    }
    return commands;
}

void DeviceSettings::setValue(const CommandMessage& response)
{
    auto setting = getSetting(response.key);
    if (setting.isValid() == false)
    {
        return;
    }

    juce::ScopedValueSetter _(ignoreCallback, true);
    if (setting[DeviceSettingsIds::value] != response.getValue())
    {
        setting.setProperty(DeviceSettingsIds::status, (int) Setting::Status::modified, nullptr);
        setting.setProperty(DeviceSettingsIds::statusTooltip, "Modified but Not Written to Device", nullptr);
    }
    setting.setProperty(DeviceSettingsIds::value, response.getValue(), nullptr);
    setting.sendPropertyChangeMessage(DeviceSettingsIds::value);
}

juce::var DeviceSettings::getValue(const juce::String& key) const
{
    auto setting = getSetting(key);
    if (setting.isValid() == false)
    {
        return {};
    }
    return setting[DeviceSettingsIds::value];
}

void DeviceSettings::setStatus(const juce::String& key, const Setting::Status status, const juce::String& statusTooltip)
{
    auto setting = getSetting(key);
    if (setting.isValid() == false)
    {
        return;
    }

    juce::ScopedValueSetter _(ignoreCallback, true);
    setting.setProperty(DeviceSettingsIds::status, (int) status, nullptr);
    setting.setProperty(DeviceSettingsIds::statusTooltip, statusTooltip, nullptr);
}

void DeviceSettings::setHideUnusedSettings(const bool hide)
{
    hideUnusedSettings = hide;
    rootItem.treeHasChanged();
}

std::vector<juce::ValueTree> DeviceSettings::flatten(const juce::ValueTree& parent)
{
    std::vector<juce::ValueTree> vector;
    for (const auto& child : parent)
    {
        for (const auto& flattened : flatten(child))
        {
            vector.push_back(flattened);
        }
        if (child.getType() == DeviceSettingsIds::Setting)
        {
            vector.push_back(child);
        }
    }
    return vector;
}

CommandMessage DeviceSettings::getWriteCommand(juce::ValueTree setting)
{
    const auto value = setting[DeviceSettingsIds::value];
    return { setting[DeviceSettingsIds::key], (setting[DeviceSettingsIds::type] == "bool") ? juce::var((bool) value) : value };
}

juce::ValueTree DeviceSettings::getSetting(const juce::String& key) const
{
    for (auto setting : settingsFlattened)
    {
        if (CommandMessage::normaliseKey(setting[DeviceSettingsIds::key]) == CommandMessage::normaliseKey(key))
        {
            return setting;
        }
    }
    return {};
}

void DeviceSettings::valueTreePropertyChanged(juce::ValueTree& tree_, const juce::Identifier& identifier)
{
    rootItem.treeHasChanged();

    if ((ignoreCallback == false) && (identifier == DeviceSettingsIds::value))
    {
        juce::NullCheckedInvocation::invoke(onSettingModified, getWriteCommand(tree_));
    }
}
