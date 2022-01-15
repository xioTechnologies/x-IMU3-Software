#include "../CustomLookAndFeel.h"
#include "CustomToggleButton.h"

class CustomToggleButton::LookAndFeel : public CustomLookAndFeel
{
public:
    using CustomLookAndFeel::CustomLookAndFeel;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        const auto font = UIFonts::defaultFont;
        const auto fontSize = font.getHeight();
        const auto tickWidth = fontSize * 1.1f;

        drawTickBox(g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
                    tickWidth, tickWidth,
                    button.getToggleState(),
                    button.isEnabled(),
                    shouldDrawButtonAsHighlighted,
                    shouldDrawButtonAsDown);

        g.setColour(button.findColour(juce::ToggleButton::textColourId));
        g.setFont(font);

        if (!button.isEnabled())
        {
            g.setOpacity(0.5f);
        }

        g.drawText(button.getButtonText(), button.getLocalBounds().withTrimmedLeft(juce::roundToInt(tickWidth) + 10).withTrimmedRight(2),
                   juce::Justification::centredLeft, true);
    }

    void drawTickBox(juce::Graphics& g,
                     juce::Component& component,
                     float x,
                     float y,
                     float w,
                     float h,
                     const bool ticked,
                     const bool isEnabled,
                     const bool shouldDrawButtonAsHighlighted,
                     const bool shouldDrawButtonAsDown) override
    {
        juce::ignoreUnused(isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        const auto tickBounds = juce::Rectangle<float>(x, y, w, h).reduced(2.0f);

        g.setColour(component.findColour(juce::ToggleButton::tickDisabledColourId));

        if (!ticked)
        {
            g.drawRoundedRectangle(tickBounds.reduced(1.0f), 2.0f, 1.5f);
        }
        else
        {
            g.fillRoundedRectangle(tickBounds, 3.0f);

            g.setColour(component.findColour(juce::ToggleButton::tickColourId));
            const auto tick = getTickShape(0.75f);
            g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(1, 2).toFloat(), false));
        }
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};

CustomToggleButton::CustomToggleButton(const juce::String& buttonName) : juce::ToggleButton(buttonName)
{
    lookAndFeel = std::make_unique<LookAndFeel>();
    setLookAndFeel(lookAndFeel.get());
    setWantsKeyboardFocus(false);
}

CustomToggleButton::~CustomToggleButton()
{
    setLookAndFeel(nullptr);
}
