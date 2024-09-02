#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

struct Overlay : juce::Component
{
    Overlay(const bool shouldInterceptMouseClicks = true)
    {
        setInterceptsMouseClicks(shouldInterceptMouseClicks, shouldInterceptMouseClicks);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black.withAlpha(0.25f));
    }
};
