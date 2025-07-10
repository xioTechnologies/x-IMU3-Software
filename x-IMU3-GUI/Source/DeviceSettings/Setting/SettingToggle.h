#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SettingEnum.h"

class SettingToggle : public SettingEnum
{
public:
    explicit SettingToggle(const juce::ValueTree& settingTree) : SettingEnum(settingTree, {
                                                                                 DeviceSettingsIds::Enum, {},
                                                                                 {
                                                                                     { DeviceSettingsIds::Enumerator, { { DeviceSettingsIds::name, "Disabled" }, { DeviceSettingsIds::value, 0 } } },
                                                                                     { DeviceSettingsIds::Enumerator, { { DeviceSettingsIds::name, "Enabled" }, { DeviceSettingsIds::value, 1 } } },
                                                                                 }
                                                                             })
    {
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingToggle)
};
