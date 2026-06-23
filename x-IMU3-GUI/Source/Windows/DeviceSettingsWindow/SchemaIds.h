#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace SchemaIds {
#define DECLARE_ID(name) static const juce::Identifier name (#name);

    DECLARE_ID(DeviceSettings)
    DECLARE_ID(Settings)
    DECLARE_ID(Setting)
    DECLARE_ID(Group)
    DECLARE_ID(Enums)
    DECLARE_ID(Enum)
    DECLARE_ID(Enumerator)

    DECLARE_ID(key)
    DECLARE_ID(name)
    DECLARE_ID(type)
    DECLARE_ID(readOnly)
    DECLARE_ID(emptyString)
    DECLARE_ID(expand)
    DECLARE_ID(dependsOnKey)
    DECLARE_ID(dependsOnValues)
    DECLARE_ID(value)

#undef DECLARE_ID
}
