#include "../CustomLookAndFeel.h"
#include "DialogButton.h"

class DialogButton::LookAndFeel : public CustomLookAndFeel
{
public:
    LookAndFeel()
    {
        setColour(juce::TextButton::buttonColourId, {});
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override
    {
        CustomLookAndFeel::drawButtonBackground(g, button, backgroundColour, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        const auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
        g.setColour(UIColours::textEditor.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.3f));
        g.drawRoundedRectangle(bounds, 2.0f, 1.0f);
    }

    juce::Font getTextButtonFont(juce::TextButton& button, int height) override
    {
        return CustomLookAndFeel::getTextButtonFont(button, juce::roundToInt((float) height * 0.8f));
    }

    int getTextButtonWidthToFitText(juce::TextButton& b, int buttonHeight) override
    {
        const auto baseWidth = CustomLookAndFeel::getTextButtonWidthToFitText(b, buttonHeight);
        static constexpr int minimumWidth = 70;
        return juce::jmax(minimumWidth, baseWidth);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};

DialogButton::DialogButton(const juce::String& buttonName) : juce::TextButton(buttonName)
{
    lookAndFeel = std::make_unique<LookAndFeel>();
    setLookAndFeel(lookAndFeel.get());
}

DialogButton::~DialogButton()
{
    setLookAndFeel(nullptr);
}
