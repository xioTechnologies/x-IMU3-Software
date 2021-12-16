#pragma once

#include "../CommandMessage.h"
#include "DeviceSettingsItem.h"
#include <juce_gui_basics/juce_gui_basics.h>

class DeviceSettings : public juce::TreeView
{
public:
    DeviceSettings();

    std::vector<CommandMessage> getReadCommands() const;

    std::vector<CommandMessage> getWriteCommands() const;

    void setValue(const CommandMessage& response);

    void setStatus(const juce::String& key, const Setting::Status status);

private:
    juce::ValueTree settingsTree = juce::ValueTree::fromXml(BinaryData::DeviceSettings_xml);
    std::vector<juce::ValueTree> settingsVector;
    DeviceSettingsItem rootItem { settingsTree };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettings)
};
