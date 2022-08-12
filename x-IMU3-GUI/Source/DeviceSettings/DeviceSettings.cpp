#include "DeviceSettings.h"
#include <regex>

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
        commands.push_back({ setting.getProperty(DeviceSettingsIDs::key), {}});
    }
    return commands;
}

std::vector<CommandMessage> DeviceSettings::getWriteCommands() const
{
    std::vector<CommandMessage> commands;
    for (auto setting : settingsVector)
    {
        if (setting.getProperty(DeviceSettingsIDs::readOnly) || setting.hasProperty(DeviceSettingsIDs::value) == false)
        {
            continue;
        }

        if (setting.getProperty(DeviceSettingsIDs::type) == "bool")
        {
            commands.push_back({ setting.getProperty(DeviceSettingsIDs::key), setting.getProperty(DeviceSettingsIDs::value) ? true : false });
        }
        else
        {
            commands.push_back({ setting.getProperty(DeviceSettingsIDs::key), setting.getProperty(DeviceSettingsIDs::value) });
        }
    }
    return commands;
}

void DeviceSettings::setValue(const CommandMessage& response)
{
    for (auto setting : settingsVector)
    {
        if (normaliseKey(setting.getProperty(DeviceSettingsIDs::key)) != normaliseKey(response.key))
        {
            continue;
        }

        if (setting[DeviceSettingsIDs::value] == response.value)
        {
            return;
        }

        setting.setProperty(DeviceSettingsIDs::value, response.value, nullptr);
        setting.setProperty(DeviceSettingsIDs::status, (int) Setting::Status::modified, nullptr);
        return;
    }
}

void DeviceSettings::setStatus(const juce::String& key, const Setting::Status status)
{
    for (auto setting : settingsVector)
    {
        if (normaliseKey(setting.getProperty(DeviceSettingsIDs::key)) == normaliseKey(key))
        {
            setting.setProperty(DeviceSettingsIDs::status, (int) status, nullptr);
            return;
        }
    }
}

juce::String DeviceSettings::normaliseKey(const juce::String& key)
{
    return std::regex_replace(key.toLowerCase().toStdString(), std::regex("[^0-9a-z]"), "");
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

void DeviceSettings::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&)
{
    rootItem.treeHasChanged();
}
