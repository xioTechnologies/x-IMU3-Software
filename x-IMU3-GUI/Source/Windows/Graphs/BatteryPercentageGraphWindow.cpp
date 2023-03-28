// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "BatteryPercentageGraphWindow.h"

Graph::Settings BatteryPercentageGraphWindow::settings = Graph::Settings(true);

BatteryPercentageGraphWindow::BatteryPercentageGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type, devicePanel_, glRenderer, "Percentage (%)", {{{}, juce::Colours::yellow }}, settings)
{
    callbackIDs.push_back(devicePanel.getConnection().addBatteryCallback(batteryCallback = [&](auto message)
    {
        update(message.timestamp, { message.percentage });
    }));
}

BatteryPercentageGraphWindow::~BatteryPercentageGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection().removeCallback(callbackID);
    }
}
