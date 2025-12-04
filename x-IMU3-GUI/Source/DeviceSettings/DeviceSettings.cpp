#include "DeviceSettings.h"
#include "KeyCompare.h"

DeviceSettings::DeviceSettings(juce::ValueTree tree, const OnSettingModified& onSettingModified_) : onSettingModified(onSettingModified_), settingsTree(tree.getChildWithName(DeviceSettingsIds::Settings)), rootItem(settingsTree, settingsFlattened, tree.getChildWithName(DeviceSettingsIds::Enums), hideUnusedSettings)
{
    setRootItem(&rootItem);
    setRootItemVisible(false);

    settingsTree.addListener(this);
}

std::vector<std::string> DeviceSettings::getReadKeys() const
{
    std::vector<std::string> keys;
    for (auto setting : settingsFlattened)
    {
        keys.push_back(setting[DeviceSettingsIds::key].toString().toStdString());
    }
    return keys;
}

std::vector<std::string> DeviceSettings::getReadCommands() const
{
    std::vector<std::string> commands;
    for (auto key : getReadKeys())
    {
        commands.push_back("{\"" + key + "\":null}");
    }
    return commands;
}

std::vector<std::string> DeviceSettings::getWriteKeys() const
{
    std::vector<std::string> keys;
    for (auto setting : settingsFlattened)
    {
        if (setting.hasProperty(DeviceSettingsIds::value) == false)
        {
            continue;
        }

        keys.push_back(setting[DeviceSettingsIds::key].toString().toStdString());
    }
    return keys;
}

std::vector<std::string> DeviceSettings::getWriteValues(const bool replaceReadOnlyValuesWithNull) const
{
    std::vector<std::string> values;

    for (auto setting : settingsFlattened)
    {
        if (setting.hasProperty(DeviceSettingsIds::value) == false)
        {
            continue;
        }

        if (setting[DeviceSettingsIds::readOnly] && replaceReadOnlyValuesWithNull)
        {
            values.push_back("null");
            continue;
        }

        values.push_back(getValue(setting));
    }

    return values;
}

std::vector<std::string> DeviceSettings::getWriteCommands(const bool replaceReadOnlyValuesWithNull) const
{
    const auto keys = getWriteKeys();
    const auto values = getWriteValues(replaceReadOnlyValuesWithNull);
    jassert(keys.size() == values.size());

    std::vector<std::string> commands;
    for (size_t index = 0; index < keys.size(); index++)
    {
        commands.push_back("{\"" + keys[index] + "\":" + values[index] + "}");
    }
    return commands;
}

void DeviceSettings::setValue(const std::string& key, const juce::var& value)
{
    auto setting = getSetting(key);
    if (setting.isValid() == false)
    {
        return;
    }

    juce::ScopedValueSetter _(ignoreCallback, true);
    if (setting[DeviceSettingsIds::value] != value)
    {
        setting.setProperty(DeviceSettingsIds::status, (int) Setting::Status::modified, nullptr);
        setting.setProperty(DeviceSettingsIds::statusTooltip, "Modified but Not Written to Device", nullptr);
    }
    setting.setProperty(DeviceSettingsIds::value, value, nullptr);
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

juce::var DeviceSettings::valueToVar(const std::string& value)
{
    return juce::JSON::fromString(value);
}

std::string DeviceSettings::varToValue(const juce::var& var)
{
    if (var.isBool())
    {
        return var ? "true" : "false";
    }

    if (var.isString())
    {
        return "\"" + var.toString().toStdString() + "\"";
    }

    return var.toString().toStdString();
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

std::string DeviceSettings::getValue(juce::ValueTree setting)
{
    return varToValue(setting[DeviceSettingsIds::value]);
}

std::string DeviceSettings::getWriteCommand(juce::ValueTree setting)
{
    return "{\"" + setting[DeviceSettingsIds::key].toString().toStdString() + "\":" + getValue(setting) + "}";
}

juce::ValueTree DeviceSettings::getSetting(const juce::String& key) const
{
    for (auto setting : settingsFlattened)
    {
        if (KeyCompare::compare(setting[DeviceSettingsIds::key], key))
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
        juce::NullCheckedInvocation::invoke(onSettingModified, tree_[DeviceSettingsIds::key].toString().toStdString(), getWriteCommand(tree_));
    }
}
