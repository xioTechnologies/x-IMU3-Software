// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "Convert.h"
#include "BatteryVoltageGraphWindow.h"

juce::ValueTree BatteryVoltageGraphWindow::settingsTree_("BatteryVoltageGraphSettings");

BatteryVoltageGraphWindow::BatteryVoltageGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, connectionPanel_,
                      glRenderer,
                      "Voltage (V)",
                      { "" },
                      { UIColours::graphChannel1 },
                      settingsTree_,
                      true)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)
    {
        update(message.timestamp, { message.voltage });
    }));
}

BatteryVoltageGraphWindow::~BatteryVoltageGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
