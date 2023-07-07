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
    for (auto setting : settingsVector)
    {
        commands.push_back({ setting[DeviceSettingsIDs::key], {}});
    }
    return commands;
}

std::vector<CommandMessage> DeviceSettings::getWriteCommands(const bool skipReadOnly) const
{
    std::vector<CommandMessage> commands;
    for (auto setting : settingsVector)
    {
        if (setting.hasProperty(DeviceSettingsIDs::value) == false)
        {
            continue;
        }

        if (setting[DeviceSettingsIDs::readOnly] && skipReadOnly)
        {
            continue;
        }

        const auto value = setting[DeviceSettingsIDs::value];
        commands.push_back({ setting[DeviceSettingsIDs::key], (setting[DeviceSettingsIDs::type] == "bool") ? juce::var((bool) value) : value });
    }
    return commands;
}

void DeviceSettings::setValue(const CommandMessage& response)
{
    auto setting = findSetting(response.key);
    if (setting.isValid() == false)
    {
        return;
    }

    if (setting[DeviceSettingsIDs::value] != response.value)
    {
        setting.setProperty(DeviceSettingsIDs::status, (int) Setting::Status::modified, nullptr);
    }
    setting.setProperty(DeviceSettingsIDs::value, response.value, nullptr);
    setting.sendPropertyChangeMessage(DeviceSettingsIDs::value);
}

void DeviceSettings::setStatus(const juce::String& key, const Setting::Status status)
{
    auto setting = findSetting(key);
    if (setting.isValid() == false)
    {
        return;
    }

    setting.setProperty(DeviceSettingsIDs::status, (int) status, nullptr);
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
        if (child.getNumChildren() == 0)
        {
            vector.push_back(child);
        }
    }
    return vector;
}

juce::ValueTree DeviceSettings::findSetting(const juce::String& key) const
{
    for (auto setting : settingsVector)
    {
        if (CommandMessage::normaliseKey(setting[DeviceSettingsIDs::key]) == CommandMessage::normaliseKey(key))
        {
            return setting;
        }
    }
    return {};
}

void DeviceSettings::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&)
{
    rootItem.treeHasChanged();
}
