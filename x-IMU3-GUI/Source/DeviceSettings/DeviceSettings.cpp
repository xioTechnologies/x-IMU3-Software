#include "DeviceSettings.h"

DeviceSettings::DeviceSettings()
{
    setRootItem(&rootItem);
    setRootItemVisible(false);
    setDefaultOpenness(true);

    settingsTree.addListener(this);
    ApplicationSettings::getSingleton().getTree().addListener(this);
}

DeviceSettings::~DeviceSettings()
{
    ApplicationSettings::getSingleton().getTree().removeListener(this);
}

std::vector<CommandMessage> DeviceSettings::getReadCommands() const
{
    std::vector<CommandMessage> commands;
    for (auto setting : settingsMap)
    {
        commands.push_back({ setting.first, {}});
    }
    return commands;
}

std::vector<CommandMessage> DeviceSettings::getWriteCommands(const bool skipReadOnly) const
{
    std::vector<CommandMessage> commands;
    for (auto setting : settingsMap)
    {
        if (setting.second.hasProperty(DeviceSettingsIDs::value) == false)
        {
            continue;
        }

        if (setting.second[DeviceSettingsIDs::readOnly] && skipReadOnly)
        {
            continue;
        }

        const auto value = setting.second[DeviceSettingsIDs::value];
        commands.push_back({ setting.first, (setting.second[DeviceSettingsIDs::type] == "bool") ? juce::var((bool) value) : value });
    }
    return commands;
}

void DeviceSettings::setValue(const CommandMessage& response)
{
    auto setting = settingsMap[CommandMessage::normaliseKey(response.key)];
    if (setting.isValid() == false)
    {
        return;
    }

    juce::ScopedValueSetter _(ignoreCallback, true);
    if (setting[DeviceSettingsIDs::value] != response.value)
    {
        setting.setProperty(DeviceSettingsIDs::status, (int) Setting::Status::modified, nullptr);
    }
    setting.setProperty(DeviceSettingsIDs::value, response.value, nullptr);
    setting.sendPropertyChangeMessage(DeviceSettingsIDs::value);
}

juce::var DeviceSettings::getValue(const juce::String& key) const
{
    if (auto it = settingsMap.find(CommandMessage::normaliseKey(key)); it != settingsMap.end())
    {
        return it->second[DeviceSettingsIDs::value];
    }
    return {};
}

void DeviceSettings::setStatus(const juce::String& key, const Setting::Status status)
{
    auto setting = settingsMap[CommandMessage::normaliseKey(key)];
    if (setting.isValid() == false)
    {
        return;
    }

    juce::ScopedValueSetter _(ignoreCallback, true);
    setting.setProperty(DeviceSettingsIDs::status, (int) status, nullptr);
}

std::map<juce::String, juce::ValueTree> DeviceSettings::flatten(const juce::ValueTree& parent)
{
    std::map<juce::String, juce::ValueTree> map;
    for (const auto& child : parent)
    {
        for (const auto& flattened : flatten(child))
        {
            map[flattened.first] = flattened.second;
        }
        if (child.getType() == DeviceSettingsIDs::Setting)
        {
            map[CommandMessage::normaliseKey(child["key"])] = child;
        }
    }
    return map;
}

void DeviceSettings::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& identifier)
{
    rootItem.treeHasChanged();

    if ((ignoreCallback == false) && (identifier == DeviceSettingsIDs::value) && ApplicationSettings::getSingleton().deviceSettings.writeSettingsWhenValueIsModified)
    {
        juce::NullCheckedInvocation::invoke(onChange);
    }
}
