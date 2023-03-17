#pragma once

class IconAndText : public juce::Component,
                    private juce::Timer
{
public:
    static constexpr int minimumWidth = 18;
    static constexpr int maximumWidth = 60;

    IconAndText(const juce::String& unknownIcon_, const juce::String& tooltip) : unknownIcon(unknownIcon_), icon(unknownIcon, tooltip)
    {
        addAndMakeVisible(icon);
        addAndMakeVisible(text);

        timerCallback();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        icon.setBounds(bounds.removeFromLeft(minimumWidth));
        bounds.reduce(5, 0);
        text.setBounds(bounds);
    }

    void update(const juce::String& icon_, const juce::String& text_)
    {
        juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this), icon_, text_]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            icon.setIcon(icon_);
                                            text.setText(text_);
                                            startTimer(3000);
                                        });
    }

private:
    const juce::String unknownIcon;
    Icon icon;
    SimpleLabel text;

    void timerCallback() override
    {
        icon.setIcon(unknownIcon);
        text.setText("");
    }
};
