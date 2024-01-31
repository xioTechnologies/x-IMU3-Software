#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <BinaryData.h>
#include "Ximu3.hpp"

class Icon : public juce::Component,
             public juce::SettableTooltipClient
{
public:
    Icon(const juce::String& icon_, const juce::String& tooltip, const float scale_ = 1.0f)
            : scale(scale_)
    {
        setIcon(icon_);
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
        if (auto xml = juce::XmlDocument::parse(icon_))
        {
            icon = juce::Drawable::createFromSVG(*xml);
        }
        repaint();
    }

private:
    std::unique_ptr<juce::Drawable> icon;
    const float scale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Icon)
};

class RssiIcon : public Icon
{
public:
    const juce::String title = "Wi-Fi RSSI";

    RssiIcon(const float scale = 1.0f) : Icon("", "", scale)
    {
        unavailable();
    }

    void unavailable()
    {
        setIcon(BinaryData::wifi_unavailable_svg);
        setTooltip(title + " (Unavailable)");
    }

    void update(const int percentage)
    {
        setIcon(percentage <= 25 ? BinaryData::wifi_25_svg :
                percentage <= 50 ? BinaryData::wifi_50_svg :
                percentage <= 75 ? BinaryData::wifi_75_svg :
                BinaryData::wifi_100_svg);
        setTooltip(title + " (" + juce::String(percentage) + "%)");
    }
};

class BatteryIcon : public Icon
{
public:
    const juce::String title = "Battery Level";

    BatteryIcon(const float scale = 1.0f) : Icon("", "", scale)
    {
        unavailable();
    }

    void unavailable()
    {
        setIcon(BinaryData::battery_unavailable_svg);
        setTooltip(title + " (Unavailable)");
    }

    void update(const int percentage, const ximu3::XIMU3_ChargingStatus status)
    {
        switch (status)
        {
            case ximu3::XIMU3_ChargingStatusNotConnected:
                setIcon(percentage <= 25 ? BinaryData::battery_25_svg :
                        percentage <= 50 ? BinaryData::battery_50_svg :
                        percentage <= 75 ? BinaryData::battery_75_svg :
                        BinaryData::battery_100_svg);
                setTooltip(title + " (" + juce::String(percentage) + "%)");
                break;

            case ximu3::XIMU3_ChargingStatusCharging:
                setIcon(BinaryData::battery_charging_svg);
                setTooltip(title + " (Charging)");
                break;

            case ximu3::XIMU3_ChargingStatusChargingComplete:
                setIcon(BinaryData::battery_charging_complete_svg);
                setTooltip(title + " (Charging Complete)");
                break;
        }
    }
};
