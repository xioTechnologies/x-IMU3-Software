// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "../../Convert.h"
#include "TemperatureGraphWindow.h"

Graph::Settings TemperatureGraphWindow::settings = Graph::Settings(true);

TemperatureGraphWindow::TemperatureGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, devicePanel_, glRenderer, "Temperature (" + degreeSymbol + "C)", {{{}, juce::Colours::yellow }}, settings, Graph::Settings(true))
{
    callbackIDs.push_back(devicePanel.getConnection()->addTemperatureCallback(temperatureCallback = [&](auto message)
    {
        update(message.timestamp, { message.temperature });
    }));
}

TemperatureGraphWindow::~TemperatureGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection()->removeCallback(callbackID);
    }
}
