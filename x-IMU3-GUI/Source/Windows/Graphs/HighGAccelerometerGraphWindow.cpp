// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "HighGAccelerometerGraphWindow.h"

Graph::Settings HighGAccelerometerGraphWindow::settings = Graph::Settings(false);

HighGAccelerometerGraphWindow::HighGAccelerometerGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type, devicePanel_, glRenderer, "Acceleration (g)", {{ "X", UIColours::graphRed },
                                                                                                                  { "Y", UIColours::graphGreen },
                                                                                                                  { "Z", UIColours::graphBlue }}, settings)
{
    callbackIDs.push_back(devicePanel.getConnection().addHighGAccelerometerCallback(highGAccelerometerCallback = [&](auto message)
    {
        update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });
    }));
}

HighGAccelerometerGraphWindow::~HighGAccelerometerGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection().removeCallback(callbackID);
    }
}
