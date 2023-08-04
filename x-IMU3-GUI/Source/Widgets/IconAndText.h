#pragma once

#include "SimpleLabel.h"

class IconAndText : public juce::Component,
                    private juce::Timer
{
public:
    static constexpr int minimumWidth = 18;
    static constexpr int maximumWidth = 60;

    IconAndText(const juce::String& unknownIcon_, const juce::String& tooltip_) : unknownIcon(unknownIcon_), tooltip(tooltip_), icon(unknownIcon, tooltip)
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
                                            setText(text_);
                                            startTimer(3000);
                                        });
    }

private:
    const juce::String unknownIcon;
    const juce::String tooltip;
    Icon icon;
    SimpleLabel text;

    void setText(const juce::String& text_)
    {
        icon.setTooltip(tooltip + (text_.isNotEmpty() ? (" (" + text_ + ")") : ""));
        text.setText(text_);
    }

    void timerCallback() override
    {
        icon.setIcon(unknownIcon);
        setText("");
    }
};
