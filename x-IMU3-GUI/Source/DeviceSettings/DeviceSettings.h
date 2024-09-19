#pragma once

#include "CommandMessage.h"
#include "DeviceSettingsItem.h"
#include <juce_gui_basics/juce_gui_basics.h>

class DeviceSettings : public juce::TreeView,
                       private juce::ValueTree::Listener
{
public:
    DeviceSettings();

    ~DeviceSettings() override;

    std::vector<CommandMessage> getReadCommands() const;

    std::vector<CommandMessage> getWriteCommands(const bool replaceReadOnlyValuesWithNull) const;

    void setValue(const CommandMessage& response);

    juce::var getValue(const juce::String& key) const;

    void setStatus(const juce::String& key, const Setting::Status status, const juce::String& statusTooltip);

    std::function<void(const CommandMessage&)> onSettingModified;

private:
    juce::ValueTree tree = juce::ValueTree::fromXml(BinaryData::DeviceSettings_xml);
    const std::vector<juce::ValueTree> settings = flatten(tree);
    DeviceSettingsItem rootItem { tree, settings };

    bool ignoreCallback = false;

    static std::vector<juce::ValueTree> flatten(const juce::ValueTree& parent);

    static CommandMessage getWriteCommand(juce::ValueTree setting);

    juce::ValueTree getSetting(const juce::String& key) const;

    void valueTreePropertyChanged(juce::ValueTree& tree_, const juce::Identifier&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettings)
};
