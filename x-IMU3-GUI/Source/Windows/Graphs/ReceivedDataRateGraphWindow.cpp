// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "Convert.h"
#include "ReceivedDataRateGraphWindow.h"

juce::ValueTree ReceivedDataRateGraphWindow::settingsTree_("ReceivedDataRateGraphSettings");

ReceivedDataRateGraphWindow::ReceivedDataRateGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, connectionPanel_,
                      glRenderer,
                      "Throughput (kB/s)",
                      { "" },
                      { UIColours::graphChannel1 },
                      settingsTree_,
                      true)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addStatisticsCallback(statisticsCallback = [&](auto message)
    {
        update(message.timestamp, { (float) message.data_rate / 1000.0f });
    }));
}

ReceivedDataRateGraphWindow::~ReceivedDataRateGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
