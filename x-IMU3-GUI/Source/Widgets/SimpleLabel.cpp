#include "SimpleLabel.h"

SimpleLabel::SimpleLabel(const juce::String& text_, const juce::Font& font_, juce::Justification justification_, const juce::URL& url_)
        : text(text_),
          font(font_),
          justification(justification_),
          url(url_)
{
    setSize((int) std::ceil(getTextWidth()), 30);

    setInterceptsMouseClicks(url.isEmpty() == false, false);

    if (url.isEmpty() == false)
    {
        setColour(juce::Label::textColourId, UIColours::hyperlink);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
        setTooltip(url.toString(false));
    }
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
    g.setColour(findColour(juce::Label::textColourId));
    g.setFont(font);

    const auto area = getLocalBounds();
    g.drawFittedText(text, area, justification, 1, 1.0f);
}

void SimpleLabel::mouseUp(const juce::MouseEvent&)
{
    if (url.isEmpty() == false)
    {
        url.launchInDefaultBrowser();
    }
}
