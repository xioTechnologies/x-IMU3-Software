// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "BatteryPercentageGraphWindow.h"

BatteryPercentageGraphWindow::BatteryPercentageGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
    : GraphWindow(windowLayout_, type_, connectionPanel_,
                  glRenderer,
                  "Percentage (%)",
                  { "" },
                  { UIColours::graphChannel1 },
                  true)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)
    {
        update(message.timestamp, { message.percentage });
    }));
}

BatteryPercentageGraphWindow::~BatteryPercentageGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
