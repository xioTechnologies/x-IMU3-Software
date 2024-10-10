// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "TemperatureGraphWindow.h"

TemperatureGraphWindow::TemperatureGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
    : GraphWindow(windowLayout_, type_, connectionPanel_,
                  glRenderer,
                  "Temperature (" + degreeSymbol + "C)",
                  { "" },
                  { UIColours::graphChannel1 },
                  true)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addTemperatureCallback(temperatureCallback = [&](auto message)
    {
        update(message.timestamp, { message.temperature });
    }));
}

TemperatureGraphWindow::~TemperatureGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
