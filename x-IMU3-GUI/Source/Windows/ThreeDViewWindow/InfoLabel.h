#pragma once

#include "CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/SimpleLabel.h"

class InfoLabel : public SimpleLabel {
public:
    InfoLabel() : SimpleLabel("", UIFonts::getDefaultFont(), juce::Justification::centred) {
    }

    void paint(juce::Graphics &g) override {
        if (getText().isEmpty()) {
            return;
        }

        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillRect(getLocalBounds().toFloat().withSizeKeepingCentre(getTextWidth() + 10.0f, 25.0f));

        SimpleLabel::paint(g);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InfoLabel)
};
