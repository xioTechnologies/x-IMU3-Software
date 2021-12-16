#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Icon : public juce::Component,
             public juce::SettableTooltipClient
{
public:
    Icon(const juce::String& icon_, const juce::String& tooltip, const juce::URL& url_ = {})
            : icon(juce::Drawable::createFromSVG(*juce::XmlDocument::parse(icon_))),
              url(url_)
    {
        setTooltip(tooltip);
        if (url.isEmpty() == false)
        {
            setMouseCursor(juce::MouseCursor::PointingHandCursor);
        }
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

    void mouseUp(const juce::MouseEvent&) override
    {
        if (url.isEmpty() == false)
        {
            url.launchInDefaultBrowser();
        }
    }

private:
    std::unique_ptr<juce::Drawable> icon;
    const juce::URL url;
};
