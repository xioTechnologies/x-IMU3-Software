#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <regex>

namespace KeyCompare
{
    inline bool compare(const juce::String& first, const juce::String& second)
    {
        const auto normalise = [] (auto key)
        {
            return std::regex_replace(key.toLowerCase().toStdString(), std::regex ("[^0-9a-z]"), "");
        };

        return normalise(first) == normalise(second);
    }
}
