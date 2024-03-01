// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "AccelerometerGraphWindow.h"

AccelerometerGraphWindow::AccelerometerGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout_, type_, connectionPanel_,
                      glRenderer,
                      "Acceleration (g)",
                      { "X", "Y", "Z" },
                      { UIColours::graphX, UIColours::graphY, UIColours::graphZ },
                      false)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addInertialCallback(inertialCallback = [&](auto message)
    {
        update(message.timestamp, { message.accelerometer_x, message.accelerometer_y, message.accelerometer_z });
    }));
}

AccelerometerGraphWindow::~AccelerometerGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
