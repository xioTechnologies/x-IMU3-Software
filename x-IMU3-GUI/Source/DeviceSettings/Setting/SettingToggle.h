#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SettingEnum.h"

class SettingToggle : public SettingEnum
{
public:
    explicit SettingToggle(const juce::ValueTree& settingTree) : SettingEnum(settingTree, { DeviceSettingsIDs::Enum, {},
                                                                                            {
                                                                                                    { DeviceSettingsIDs::Enumerator, {{ DeviceSettingsIDs::name, "Disabled" }, { DeviceSettingsIDs::value, 0 }}},
                                                                                                    { DeviceSettingsIDs::Enumerator, {{ DeviceSettingsIDs::name, "Enabled" }, { DeviceSettingsIDs::value, 1 }}},
                                                                                            }})
    {
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingToggle)
};
