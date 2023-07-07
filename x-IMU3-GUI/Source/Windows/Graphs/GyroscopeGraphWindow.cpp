// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "../../Convert.h"
#include "GyroscopeGraphWindow.h"

Graph::Settings GyroscopeGraphWindow::settings = Graph::Settings(false);

GyroscopeGraphWindow::GyroscopeGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, devicePanel_, glRenderer, "Angular velocity (" + degreeSymbol + "/s)", {{ "X", UIColours::graphRed },
                                                                                                                   { "Y", UIColours::graphGreen },
                                                                                                                   { "Z", UIColours::graphBlue }}, settings, Graph::Settings(false))
{
    callbackIDs.push_back(devicePanel.getConnection()->addInertialCallback(inertialCallback = [&](auto message)
    {
        update(message.timestamp, { message.gyroscope_x, message.gyroscope_y, message.gyroscope_z });
    }));
}

GyroscopeGraphWindow::~GyroscopeGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection()->removeCallback(callbackID);
    }
}
