#include "DeviceSettings.h"

DeviceSettings::DeviceSettings(juce::ValueTree tree, std::function<void(const CommandMessage&)> onSettingModified_) : onSettingModified(onSettingModified_), settingsTree(tree.getChildWithName(DeviceSettingsIDs::Settings)), rootItem(settingsTree, settingsFlattened, tree.getChildWithName(DeviceSettingsIDs::Enums), hideUnusedSettings)
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
        commands.push_back({ setting[DeviceSettingsIDs::key], {} });
    }
    return commands;
}

std::vector<CommandMessage> DeviceSettings::getWriteCommands(const bool replaceReadOnlyValuesWithNull) const
{
    std::vector<CommandMessage> commands;
    for (auto setting : settingsFlattened)
    {
        if (setting.hasProperty(DeviceSettingsIDs::value) == false)
        {
            continue;
        }

        if (setting[DeviceSettingsIDs::readOnly] && replaceReadOnlyValuesWithNull)
        {
            commands.push_back({ setting[DeviceSettingsIDs::key], {} });
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
    if (setting[DeviceSettingsIDs::value] != response.getValue())
    {
        setting.setProperty(DeviceSettingsIDs::status, (int) Setting::Status::modified, nullptr);
        setting.setProperty(DeviceSettingsIDs::statusTooltip, "Modified but Not Written to Device", nullptr);
    }
    setting.setProperty(DeviceSettingsIDs::value, response.getValue(), nullptr);
    setting.sendPropertyChangeMessage(DeviceSettingsIDs::value);
}

juce::var DeviceSettings::getValue(const juce::String& key) const
{
    auto setting = getSetting(key);
    if (setting.isValid() == false)
    {
        return {};
    }
    return setting[DeviceSettingsIDs::value];
}

void DeviceSettings::setStatus(const juce::String& key, const Setting::Status status, const juce::String& statusTooltip)
{
    auto setting = getSetting(key);
    if (setting.isValid() == false)
    {
        return;
    }

    juce::ScopedValueSetter _(ignoreCallback, true);
    setting.setProperty(DeviceSettingsIDs::status, (int) status, nullptr);
    setting.setProperty(DeviceSettingsIDs::statusTooltip, statusTooltip, nullptr);
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
        if (child.getType() == DeviceSettingsIDs::Setting)
        {
            vector.push_back(child);
        }
    }
    return vector;
}

CommandMessage DeviceSettings::getWriteCommand(juce::ValueTree setting)
{
    const auto value = setting[DeviceSettingsIDs::value];
    return { setting[DeviceSettingsIDs::key], (setting[DeviceSettingsIDs::type] == "bool") ? juce::var((bool) value) : value };
}

juce::ValueTree DeviceSettings::getSetting(const juce::String& key) const
{
    for (auto setting : settingsFlattened)
    {
        if (CommandMessage::normaliseKey(setting[DeviceSettingsIDs::key]) == CommandMessage::normaliseKey(key))
        {
            return setting;
        }
    }
    return {};
}

void DeviceSettings::valueTreePropertyChanged(juce::ValueTree& tree_, const juce::Identifier& identifier)
{
    if ((ignoreCallback == false) && (identifier == DeviceSettingsIDs::value))
    {
        juce::NullCheckedInvocation::invoke(onSettingModified, getWriteCommand(tree_));
    }
}
