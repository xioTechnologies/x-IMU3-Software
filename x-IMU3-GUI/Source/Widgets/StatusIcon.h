#pragma once

#include "SimpleLabel.h"

class StatusIcon : public juce::Component,
                   private juce::Timer
{
public:
    StatusIcon(const juce::String& unknownIcon_, const juce::String& tooltip_) : unknownIcon(unknownIcon_), tooltip(tooltip_), icon(unknownIcon, tooltip)
    {
        addAndMakeVisible(icon);
        addAndMakeVisible(text);

        timerCallback();
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        icon.setBounds(bounds.removeFromLeft(iconWidth));
        bounds.removeFromLeft(5);
        text.setBounds(bounds);
    }

    void update(const juce::String& icon_, const juce::String& text_, const juce::String& textVerbose)
    {
        juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this), icon_, text_, textVerbose]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            icon.setIcon(icon_);
                                            setText(text_, textVerbose);
                                            startTimer(3000);
                                        });
    }

    int getWidth(const bool showText) const
    {
        return iconWidth + ((showText && text.getText().isNotEmpty()) ? 40 : 0);
    }

private:
    static constexpr int iconWidth = 18;

    const juce::String unknownIcon;
    const juce::String tooltip;
    Icon icon;
    SimpleLabel text;

    void setText(const juce::String& text_, const juce::String& textVerbose)
    {
        const auto resizeParent = text.getText().isEmpty() || text_.isEmpty();
        icon.setTooltip(tooltip + " (" + (textVerbose.isEmpty() ? "Unavailable" : textVerbose) + ")");
        text.setText(text_);
        if (resizeParent && getParentComponent() != nullptr)
        {
            getParentComponent()->resized();
        }
    }

    void timerCallback() override
    {
        icon.setIcon(unknownIcon);
        setText("", "");
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatusIcon)
};
