#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace WindowIDs
{
#define DECLARE_ID(name) static const juce::Identifier name (#name);

    DECLARE_ID(Column)
    DECLARE_ID(Row)

    DECLARE_ID(DeviceSettings)
    DECLARE_ID(ThreeDView)
    DECLARE_ID(SerialAccessoryTerminal)
    DECLARE_ID(Gyroscope)
    DECLARE_ID(Accelerometer)
    DECLARE_ID(Magnetometer)
    DECLARE_ID(EulerAngles)
    DECLARE_ID(LinearAcceleration)
    DECLARE_ID(EarthAcceleration)
    DECLARE_ID(HighGAccelerometer)
    DECLARE_ID(Temperature)
    DECLARE_ID(BatteryPercentage)
    DECLARE_ID(BatteryVoltage)
    DECLARE_ID(RssiPercentage)
    DECLARE_ID(RssiPower)
    DECLARE_ID(ReceivedMessageRate)
    DECLARE_ID(ReceivedDataRate)

    DECLARE_ID(size)

#undef DECLARE_ID
}

inline juce::String getWindowTitle(const juce::Identifier& windowType)
{
    static std::map<juce::Identifier, juce::String> map
            {
                    { WindowIDs::Accelerometer,           "Accelerometer" },
                    { WindowIDs::BatteryPercentage,       "Battery Percentage" },
                    { WindowIDs::BatteryVoltage,          "Battery Voltage" },
                    { WindowIDs::DeviceSettings,          "Device Settings" },
                    { WindowIDs::EarthAcceleration,       "Earth Acceleration" },
                    { WindowIDs::EulerAngles,             "Euler Angles" },
                    { WindowIDs::Gyroscope,               "Gyroscope" },
                    { WindowIDs::HighGAccelerometer,      "High-g Accelerometer" },
                    { WindowIDs::LinearAcceleration,      "Linear Acceleration" },
                    { WindowIDs::Magnetometer,            "Magnetometer" },
                    { WindowIDs::ReceivedDataRate,        "Received Data Rate" },
                    { WindowIDs::ReceivedMessageRate,     "Received Message Rate" },
                    { WindowIDs::RssiPercentage,          "RSSI Percentage" },
                    { WindowIDs::RssiPower,               "RSSI Power" },
                    { WindowIDs::SerialAccessoryTerminal, "Serial Accessory Terminal" },
                    { WindowIDs::Temperature,             "Temperature" },
                    { WindowIDs::ThreeDView,              "3D View" },
            };

    return map[windowType];
}

inline juce::ValueTree findWindow(const juce::ValueTree root, const juce::Identifier& type)
{
    for (auto child : root)
    {
        if (child.hasType(type))
        {
            return child;
        }

        auto childOfChild = findWindow(child, type);
        if (childOfChild.isValid())
        {
            return childOfChild;
        }
    }
    return {};
}
