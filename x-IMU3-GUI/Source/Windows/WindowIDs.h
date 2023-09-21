#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace WindowIDs
{
#define DECLARE_ID(name) static const juce::Identifier name (#name);

    DECLARE_ID(Column)
    DECLARE_ID(Row)

    DECLARE_ID(AccelerometerGraph)
    DECLARE_ID(BatteryPercentageGraph)
    DECLARE_ID(BatteryVoltageGraph)
    DECLARE_ID(DeviceSettings)
    DECLARE_ID(EarthAccelerationGraph)
    DECLARE_ID(EulerAnglesGraph)
    DECLARE_ID(GyroscopeGraph)
    DECLARE_ID(HighGAccelerometerGraph)
    DECLARE_ID(LinearAccelerationGraph)
    DECLARE_ID(MagnetometerGraph)
    DECLARE_ID(ReceivedDataRateGraph)
    DECLARE_ID(ReceivedMessageRateGraph)
    DECLARE_ID(RssiPercentageGraph)
    DECLARE_ID(RssiPowerGraph)
    DECLARE_ID(SerialAccessoryGraph)
    DECLARE_ID(SerialAccessoryTerminal)
    DECLARE_ID(TemperatureGraph)
    DECLARE_ID(ThreeDView)

    DECLARE_ID(size)

#undef DECLARE_ID
}

inline juce::String getWindowTitle(const juce::Identifier& windowType)
{
    static std::map<juce::Identifier, juce::String> map
            {
                    { WindowIDs::AccelerometerGraph,       "Accelerometer" },
                    { WindowIDs::BatteryPercentageGraph,   "Battery Percentage" },
                    { WindowIDs::BatteryVoltageGraph,      "Battery Voltage" },
                    { WindowIDs::DeviceSettings,           "Device Settings" },
                    { WindowIDs::EarthAccelerationGraph,   "Earth Acceleration" },
                    { WindowIDs::EulerAnglesGraph,         "Euler Angles" },
                    { WindowIDs::GyroscopeGraph,           "Gyroscope" },
                    { WindowIDs::HighGAccelerometerGraph,  "High-g Accelerometer" },
                    { WindowIDs::LinearAccelerationGraph,  "Linear Acceleration" },
                    { WindowIDs::MagnetometerGraph,        "Magnetometer" },
                    { WindowIDs::ReceivedDataRateGraph,    "Received Data Rate" },
                    { WindowIDs::ReceivedMessageRateGraph, "Received Message Rate" },
                    { WindowIDs::RssiPercentageGraph,      "RSSI Percentage" },
                    { WindowIDs::RssiPowerGraph,           "RSSI Power" },
                    { WindowIDs::SerialAccessoryGraph,     "Serial Accessory Graph" },
                    { WindowIDs::SerialAccessoryTerminal,  "Serial Accessory Terminal" },
                    { WindowIDs::TemperatureGraph,         "Temperature" },
                    { WindowIDs::ThreeDView,               "3D View" },
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
