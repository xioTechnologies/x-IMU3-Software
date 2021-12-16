#pragma once

#include "CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class SimpleLabel : public juce::Component,
                    public juce::SettableTooltipClient
{
public:
    SimpleLabel(const juce::String& text_ = {},
                const juce::Font& font_ = UIFonts::defaultFont,
                juce::Justification justification_ = juce::Justification::centredLeft,
                const juce::URL& url_ = {});

    void setText(const juce::String& text_);

    const juce::String& getText() const;

    float getTextWidth() const;

    void paint(juce::Graphics& g) override;

    void mouseUp(const juce::MouseEvent&) override;

private:
    juce::String text;
    const juce::Font font;
    const juce::Justification justification;
    const juce::URL url;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleLabel)
};
