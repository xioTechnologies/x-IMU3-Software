#pragma once

#include "ApplicationSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>

class WindowLayouts
{
public:
    bool exists(const juce::String& name) const;

    void save(const juce::String& name, juce::ValueTree tree) const;

    std::map<juce::String, juce::ValueTree> load() const;

private:
    const juce::File directory = ApplicationSettings::getDirectory().getChildFile("Window Layouts");
};
