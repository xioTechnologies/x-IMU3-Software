// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "../../Convert.h"
#include "RssiPowerGraphWindow.h"

Graph::Settings RssiPowerGraphWindow::settings = Graph::Settings(true);

RssiPowerGraphWindow::RssiPowerGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, devicePanel_, glRenderer, "Power (dBm)", {{{}, juce::Colours::yellow }}, settings)
{
    callbackIDs.push_back(devicePanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)
    {
        update(message.timestamp, { message.power });
    }));
}

RssiPowerGraphWindow::~RssiPowerGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection()->removeCallback(callbackID);
    }
}
