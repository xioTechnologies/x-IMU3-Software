#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SettingEnum.h"

class SettingToggle : public SettingEnum
{
public:
    explicit SettingToggle(const juce::ValueTree& settingTree) : SettingEnum(settingTree, {{ 0, "Disabled" },
                                                                                           { 1, "Enabled" }})
    {
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingToggle)
};
