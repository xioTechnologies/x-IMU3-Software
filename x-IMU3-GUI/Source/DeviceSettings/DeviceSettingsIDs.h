#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace DeviceSettingsIDs
{
#define DECLARE_ID(name) static const juce::Identifier name (#name);

    DECLARE_ID(Setting)
    DECLARE_ID(Group)
    DECLARE_ID(Margin)
    DECLARE_ID(Enum)
    DECLARE_ID(Enumerator)

    DECLARE_ID(key)
    DECLARE_ID(name)
    DECLARE_ID(open)
    DECLARE_ID(type)
    DECLARE_ID(readOnly)
    DECLARE_ID(hideKey)
    DECLARE_ID(hideValues)
    DECLARE_ID(emptyValue)

    DECLARE_ID(value)
    DECLARE_ID(status)
    DECLARE_ID(statusTooltip)

#undef DECLARE_ID
}
