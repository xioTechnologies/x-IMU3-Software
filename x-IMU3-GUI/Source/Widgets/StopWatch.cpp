#include "CustomLookAndFeel.h"
#include "Stopwatch.h"

class Stopwatch::LookAndFeel : public CustomLookAndFeel
{
public:
    using CustomLookAndFeel::CustomLookAndFeel;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override
    {
        const auto font = getTextButtonFont(button, button.getHeight());
        g.setFont(font);
        g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                              : juce::TextButton::textColourOffId)
                          .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        g.drawText(button.getButtonText(), button.getLocalBounds().withSizeKeepingCentre(font.getStringWidth("00:00:00.000"), (int) std::ceil(font.getHeight())), juce::Justification::centredLeft);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};

Stopwatch::Stopwatch()
{
    lookAndFeel = std::make_unique<LookAndFeel>();
    setLookAndFeel(lookAndFeel.get());

    setInterceptsMouseClicks(false, false);
    setColour(juce::TextButton::buttonColourId, UIColours::backgroundDark);
    setTime(juce::RelativeTime());
}

Stopwatch::~Stopwatch()
{
    setLookAndFeel(nullptr);
}

void Stopwatch::setTime(const juce::RelativeTime& time)
{
    juce::String buttonText;
    buttonText += juce::String((int) std::floor(time.inHours()) % 100).paddedLeft('0', 2) + ":";
    buttonText += juce::String((int) std::floor(time.inMinutes()) % 60).paddedLeft('0', 2) + ":";
    buttonText += juce::String((int) std::floor(time.inSeconds()) % 60).paddedLeft('0', 2) + ".";
    buttonText += juce::String((int) std::floor((time.inMilliseconds())) % 1000).paddedLeft('0', 3);
    setButtonText(buttonText);
}
