#pragma once

#include "../CustomLookAndFeel.h"

class SimpleProgressBar : public juce::Component {
public:
    void paint(juce::Graphics &g) override {
        g.fillAll(UIColours::foreground);

        g.setColour(UIColours::highlight);
        g.fillRect(getLocalBounds().withWidth(proportionOfWidth(progress)));
    }

    void setProgress(const float progress_) {
        progress = progress_;
        repaint();
    }

private:
    float progress = 0.0f;
};
