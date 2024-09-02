#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct DisabledOverlay : juce::Component
{
    DisabledOverlay(const bool shouldInterceptMouseClicks = true)
    {
        setInterceptsMouseClicks(shouldInterceptMouseClicks, shouldInterceptMouseClicks);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.25f));
    }
};
