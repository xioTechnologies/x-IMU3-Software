// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "../../Convert.h"
#include "EarthAccelerationGraphWindow.h"

Graph::Settings EarthAccelerationGraphWindow::settings = Graph::Settings(false);

EarthAccelerationGraphWindow::EarthAccelerationGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, devicePanel_, glRenderer, "Acceleration (g)", {{ "X", UIColours::graphRed },
                                                                                          { "Y", UIColours::graphGreen },
                                                                                          { "Z", UIColours::graphBlue }}, settings)
{
    callbackIDs.push_back(devicePanel.getConnection()->addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
    {
        update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });
    }));
}

EarthAccelerationGraphWindow::~EarthAccelerationGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection()->removeCallback(callbackID);
    }
}
