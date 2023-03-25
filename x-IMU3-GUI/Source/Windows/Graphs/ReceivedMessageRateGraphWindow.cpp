// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ReceivedMessageRateGraphWindow.h"

Graph::Settings ReceivedMessageRateGraphWindow::settings = []
{
    Graph::Settings settings_;
    settings_.horizontal.autoscale = true;
    return settings_;
}();

ReceivedMessageRateGraphWindow::ReceivedMessageRateGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type, devicePanel_, glRenderer, "Receive rate (messages/s)", {{{}, juce::Colours::yellow }}, settings)
{
    callbackIDs.push_back(devicePanel.getConnection().addStatisticsCallback(statisticsCallback = [&](auto message)
    {
        update(message.timestamp, { (float) message.message_rate });
    }));
}

ReceivedMessageRateGraphWindow::~ReceivedMessageRateGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection().removeCallback(callbackID);
    }
}
