#pragma once

#include "../CommandMessage.h"
#include "DeviceSettingsItem.h"
#include <juce_gui_basics/juce_gui_basics.h>

class DeviceSettings : public juce::TreeView,
                       private juce::ValueTree::Listener
{
public:
    DeviceSettings();

    ~DeviceSettings() override;

    std::vector<CommandMessage> getReadCommands() const;

    std::vector<CommandMessage> getWriteCommands() const;

    void setValue(const CommandMessage& response);

    void setStatus(const juce::String& key, const Setting::Status status);

private:
    juce::ValueTree settingsTree = juce::ValueTree::fromXml(BinaryData::DeviceSettings_xml);
    const std::vector<juce::ValueTree> settingsVector = flatten(settingsTree);
    DeviceSettingsItem rootItem { settingsTree, settingsVector };

    static juce::String normaliseKey(const juce::String& key);

    static std::vector<juce::ValueTree> flatten(const juce::ValueTree& parent);

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettings)
};
