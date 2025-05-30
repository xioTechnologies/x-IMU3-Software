#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace WindowIds
{
#define DECLARE_ID(name) static const juce::Identifier name (#name);

    DECLARE_ID(Column)
    DECLARE_ID(Row)

    DECLARE_ID(Accelerometer)
    DECLARE_ID(BatteryPercentage)
    DECLARE_ID(BatteryVoltage)
    DECLARE_ID(DeviceSettings)
    DECLARE_ID(EarthAcceleration)
    DECLARE_ID(EulerAngles)
    DECLARE_ID(Gyroscope)
    DECLARE_ID(HighGAccelerometer)
    DECLARE_ID(LinearAcceleration)
    DECLARE_ID(Magnetometer)
    DECLARE_ID(ReceivedDataRate)
    DECLARE_ID(ReceivedMessageRate)
    DECLARE_ID(RssiPercentage)
    DECLARE_ID(RssiPower)
    DECLARE_ID(SerialAccessoryCsvs)
    DECLARE_ID(SerialAccessoryTerminal)
    DECLARE_ID(Temperature)
    DECLARE_ID(ThreeDView)

    DECLARE_ID(size)

#undef DECLARE_ID
}

static const std::map<juce::Identifier, juce::String> windowTitles
        {
                { WindowIds::Accelerometer,           "Accelerometer" },
                { WindowIds::BatteryPercentage,       "Battery Percentage" },
                { WindowIds::BatteryVoltage,          "Battery Voltage" },
                { WindowIds::DeviceSettings,          "Device Settings" },
                { WindowIds::EarthAcceleration,       "Earth Acceleration" },
                { WindowIds::EulerAngles,             "Euler Angles" },
                { WindowIds::Gyroscope,               "Gyroscope" },
                { WindowIds::HighGAccelerometer,      "High-g Accelerometer" },
                { WindowIds::LinearAcceleration,      "Linear Acceleration" },
                { WindowIds::Magnetometer,            "Magnetometer" },
                { WindowIds::ReceivedDataRate,        "Received Data Rate" },
                { WindowIds::ReceivedMessageRate,     "Received Message Rate" },
                { WindowIds::RssiPercentage,          "RSSI Percentage" },
                { WindowIds::RssiPower,               "RSSI Power" },
                { WindowIds::SerialAccessoryCsvs,     "Serial Accessory CSVs" },
                { WindowIds::SerialAccessoryTerminal, "Serial Accessory Terminal" },
                { WindowIds::Temperature,             "Temperature" },
                { WindowIds::ThreeDView,              "3D View" },
        };

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
