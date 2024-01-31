#pragma once

#include "Icon.h"

template<typename IconType>
class IconAndText : public juce::Component, protected juce::Timer
{
public:
    IconAndText()
    {
        addAndMakeVisible(icon);
        addAndMakeVisible(text);

        icon.setSize(18, 0);

        timerCallback();
        startTimer(3000);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        icon.setBounds(bounds.removeFromLeft(icon.getWidth()));
        bounds.removeFromLeft(5);
        text.setBounds(bounds);
    }

    int getWidth(const bool showText) const
    {
        return icon.getWidth() + ((showText && text.getText().isNotEmpty()) ? 40 : 0);
    }

protected:
    IconType icon;

    void setText(const juce::String& text_)
    {
        if (text.getText() != text_)
        {
            text.setText(text_);
            if (getParentComponent() != nullptr)
            {
                getParentComponent()->resized();
            }
        }
    }

    void timerCallback() override
    {
        icon.unavailable();
        setText("");
    }

private:
    SimpleLabel text;
};

class RssiIconAndText : public IconAndText<RssiIcon>
{
public:
    void update(const int percentage)
    {
        juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this), percentage]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            icon.update(percentage);
                                            setText(juce::String(percentage) + "%");
                                            startTimer(getTimerInterval());
                                        });
    }
};

class BatteryIconAndText : public IconAndText<BatteryIcon>
{
public:
    void update(const int percentage, const ximu3::XIMU3_ChargingStatus status)
    {
        juce::MessageManager::callAsync([&, self = SafePointer<juce::Component>(this), percentage, status]
                                        {
                                            if (self == nullptr)
                                            {
                                                return;
                                            }

                                            icon.update(percentage, status);
                                            switch (status)
                                            {
                                                case ximu3::XIMU3_ChargingStatusNotConnected:
                                                    setText(juce::String(percentage) + "%");
                                                    break;
                                                case ximu3::XIMU3_ChargingStatusCharging:
                                                case ximu3::XIMU3_ChargingStatusChargingComplete:
                                                    setText("USB");
                                                    break;
                                            }
                                            startTimer(getTimerInterval());
                                        });
    }
};
