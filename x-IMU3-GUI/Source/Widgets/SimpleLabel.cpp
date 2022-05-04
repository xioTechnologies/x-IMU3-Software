#include "SimpleLabel.h"

SimpleLabel::SimpleLabel(const juce::String& text_, const juce::Font& font_, juce::Justification justification_)
        : text(text_),
          font(font_),
          justification(justification_)
{
    setSize((int) std::ceil(getTextWidth()), 30);
    setInterceptsMouseClicks(false, false);
}

void SimpleLabel::setText(const juce::String& text_)
{
    if (text != text_)
    {
        text = text_;
        repaint();
    }
}

const juce::String& SimpleLabel::getText() const
{
    return text;
}

float SimpleLabel::getTextWidth() const
{
    return font.getStringWidthFloat(text);
}

void SimpleLabel::paint(juce::Graphics& g)
{
    g.setColour(findColour(juce::Label::textColourId).withMultipliedAlpha(isEnabled() ? 1.0f : 0.5f));
    g.setFont(font);
    g.drawFittedText(text, getLocalBounds(), justification, 1, 1.0f);
}
