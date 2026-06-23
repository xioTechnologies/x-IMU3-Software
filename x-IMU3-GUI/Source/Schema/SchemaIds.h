#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace SchemaIds {
#define DECLARE_ID(name) static const juce::Identifier name (#name);

    DECLARE_ID(Commands)
    DECLARE_ID(Settings)

    DECLARE_ID(name)

#undef DECLARE_ID
}
