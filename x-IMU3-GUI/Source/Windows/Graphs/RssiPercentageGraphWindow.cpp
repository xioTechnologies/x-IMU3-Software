// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "RssiPercentageGraphWindow.h"

RssiPercentageGraphWindow::RssiPercentageGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
    : GraphWindow(windowLayout_, type_, connectionPanel_,
                  glRenderer,
                  "Percentage (%)",
                  { "" },
                  { UIColours::graphChannel1 },
                  true)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)
    {
        update(message.timestamp, { message.percentage });
    }));
}

RssiPercentageGraphWindow::~RssiPercentageGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
