#pragma once

#include "DeviceSettingsItem.h"
#include <juce_gui_basics/juce_gui_basics.h>

class DeviceSettings : public juce::TreeView,
                       private juce::ValueTree::Listener
{
public:
    using OnSettingModified = std::function<void(const std::string& key, const std::string& command)>;

    DeviceSettings(juce::ValueTree tree, const OnSettingModified& onSettingModified_);

    std::vector<std::string> getReadKeys() const;

    std::vector<std::string> getReadCommands() const;

    std::vector<std::string> getWriteKeys() const;

    std::vector<std::string> getWriteValues(const bool replaceReadOnlyValuesWithNull) const;

    std::vector<std::string> getWriteCommands(const bool replaceReadOnlyValuesWithNull) const;

    void setValue(const std::string& key, const juce::var& value);

    juce::var getValue(const juce::String& key) const;

    void setStatus(const juce::String& key, const Setting::Status status, const juce::String& statusTooltip);

    void setHideUnusedSettings(const bool hide);

    static juce::var valueToVar(const std::string& value);

    static std::string varToValue(const juce::var& var);

private:
    const OnSettingModified onSettingModified;
    juce::ValueTree settingsTree;
    const std::vector<juce::ValueTree> settingsFlattened = flatten(settingsTree);
    bool hideUnusedSettings = true;
    DeviceSettingsItem rootItem;

    bool ignoreCallback = false;

    static std::vector<juce::ValueTree> flatten(const juce::ValueTree& parent);

    static std::string getValue(juce::ValueTree setting);

    static std::string getWriteCommand(juce::ValueTree setting);

    juce::ValueTree getSetting(const juce::String& key) const;

    void valueTreePropertyChanged(juce::ValueTree& tree_, const juce::Identifier&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceSettings)
};
