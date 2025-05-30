// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "TemperatureGraphWindow.h"

TemperatureGraphWindow::TemperatureGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, OpenGLRenderer& openGLRenderer)
    : GraphWindow(windowLayout_, type_, connectionPanel_,
                  openGLRenderer,
                  "Temperature (" + degreeSymbol + "C)",
                  { "" },
                  { UIColours::graphChannel1 },
                  true)
{
    callbackIds.push_back(connectionPanel.getConnection()->addTemperatureCallback(temperatureCallback = [&](auto message)
    {
        update(message.timestamp, { message.temperature });
    }));
}

TemperatureGraphWindow::~TemperatureGraphWindow()
{
    for (const auto callbackId : callbackIds)
    {
        connectionPanel.getConnection()->removeCallback(callbackId);
    }
}
