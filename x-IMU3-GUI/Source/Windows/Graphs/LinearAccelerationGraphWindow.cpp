// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "LinearAccelerationGraphWindow.h"

LinearAccelerationGraphWindow::LinearAccelerationGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
    : GraphWindow(windowLayout_, type_, connectionPanel_,
                  glRenderer,
                  "Acceleration (g)",
                  { "X", "Y", "Z" },
                  { UIColours::graphX, UIColours::graphY, UIColours::graphZ },
                  false)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)
    {
        update(message.timestamp, { message.acceleration_x, message.acceleration_y, message.acceleration_z });
    }));
}

LinearAccelerationGraphWindow::~LinearAccelerationGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
