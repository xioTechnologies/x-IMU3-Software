#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Icon : public juce::Component,
             public juce::SettableTooltipClient
{
public:
    Icon(const juce::String& icon_, const juce::String& tooltip, const float scale_ = 1.0f)
            : icon(juce::Drawable::createFromSVG(*juce::XmlDocument::parse(icon_))),
              scale(scale_)
    {
        setTooltip(tooltip);
    }

    void paint(juce::Graphics& g) override
    {
        if (icon != nullptr)
        {
            icon->drawWithin(g, getLocalBounds().toFloat().withSizeKeepingCentre(getWidth() * scale, getHeight() * scale),
                             juce::RectanglePlacement::centred, 1.0f);
        }
    }

    void setIcon(const juce::String& icon_)
    {
        icon = juce::Drawable::createFromSVG(*juce::XmlDocument::parse(icon_));
        repaint();
    }

private:
    std::unique_ptr<juce::Drawable> icon;
    const float scale;
};
