// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "EarthAccelerationGraphWindow.h"

EarthAccelerationGraphWindow::EarthAccelerationGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, OpenGLRenderer& openGLRenderer)
    : GraphWindow(windowLayout_, type_, connectionPanel_,
                  openGLRenderer,
                  "Acceleration (g)",
                  { "X", "Y", "Z" },
                  { UIColours::graphX, UIColours::graphY, UIColours::graphZ },
                  false)
{
    callbackIds.push_back(connectionPanel.getConnection()->addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
    {
        update(message.timestamp, { message.acceleration_x, message.acceleration_y, message.acceleration_z });
    }));
}

EarthAccelerationGraphWindow::~EarthAccelerationGraphWindow()
{
    for (const auto callbackId : callbackIds)
    {
        connectionPanel.getConnection()->removeCallback(callbackId);
    }
}
