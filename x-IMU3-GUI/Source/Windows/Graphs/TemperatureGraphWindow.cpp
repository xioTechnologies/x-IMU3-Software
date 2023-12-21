// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "Convert.h"
#include "TemperatureGraphWindow.h"

juce::ValueTree TemperatureGraphWindow::settingsTree_("TemperatureGraphSettings");

TemperatureGraphWindow::TemperatureGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, connectionPanel_,
                      glRenderer,
                      "Temperature (" + degreeSymbol + "C)",
                      { "" },
                      { UIColours::graphChannel1 },
                      settingsTree_,
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
