#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Icon : public juce::Component,
             public juce::SettableTooltipClient
{
public:
    Icon(const juce::String& icon_, const juce::String& tooltip)
            : icon(juce::Drawable::createFromSVG(*juce::XmlDocument::parse(icon_)))
    {
        setTooltip(tooltip);
    }

    void setIcon(const juce::String& icon_)
    {
        icon = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(icon_));
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        if (icon != nullptr)
        {
            icon->drawWithin(g, getLocalBounds().toFloat(), juce::RectanglePlacement::centred, 1.0f);
        }
    }

private:
    std::unique_ptr<juce::Drawable> icon;
    const juce::URL url;
};
