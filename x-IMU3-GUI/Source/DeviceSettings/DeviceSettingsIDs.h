#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace DeviceSettingsIDs
{
#define DECLARE_ID(name) static const juce::Identifier name (#name);

    DECLARE_ID(Group)

    DECLARE_ID(displayName)
    DECLARE_ID(enumName)
    DECLARE_ID(hideKey)
    DECLARE_ID(hideValues)
    DECLARE_ID(key)
    DECLARE_ID(readOnly)
    DECLARE_ID(status)
    DECLARE_ID(type)
    DECLARE_ID(value)

#undef DECLARE_ID
}
