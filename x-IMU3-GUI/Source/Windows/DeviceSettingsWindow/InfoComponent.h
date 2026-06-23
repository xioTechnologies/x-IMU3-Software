#pragma once

#include "CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class InfoComponent final : public juce::Component {
public:
    explicit InfoComponent(const juce::String &text, const std::optional<juce::String> &detail) {
        info.setJustification(juce::Justification::centred);
        info.append(text, UIColours::foreground);
        if (detail) {
            info.append("\n" + *detail, juce::Colours::grey);
        }
        info.setFont(UIFonts::getDefaultFont());
    }

    void paint(juce::Graphics &g) override {
        info.draw(g, getLocalBounds().toFloat());
    }

private:
    juce::AttributedString info;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoComponent)
};
