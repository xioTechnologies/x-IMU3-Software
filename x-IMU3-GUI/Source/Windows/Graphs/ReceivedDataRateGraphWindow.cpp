// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "../../Convert.h"
#include "../../DevicePanel/DevicePanel.h"
#include "ReceivedDataRateGraphWindow.h"

juce::ValueTree ReceivedDataRateGraphWindow::settingsTree_("ReceivedDataRateGraphSettings");

ReceivedDataRateGraphWindow::ReceivedDataRateGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, devicePanel_,
                      glRenderer,
                      "Throughput (kB/s)",
                      { "" },
                      { UIColours::graphChannel1 },
                      settingsTree_,
                      true)
{
    callbackIDs.push_back(devicePanel.getConnection()->addStatisticsCallback(statisticsCallback = [&](auto message)
    {
        update(message.timestamp, { (float) message.data_rate / 1000.0f });
    }));
}

ReceivedDataRateGraphWindow::~ReceivedDataRateGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection()->removeCallback(callbackID);
    }
}
