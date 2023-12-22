// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "Convert.h"
#include "HighGAccelerometerGraphWindow.h"

HighGAccelerometerGraphWindow::HighGAccelerometerGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, connectionPanel_,
                      glRenderer,
                      "Acceleration (g)",
                      { "X", "Y", "Z" },
                      { UIColours::graphX, UIColours::graphY, UIColours::graphZ },
                      false)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addHighGAccelerometerCallback(highGAccelerometerCallback = [&](auto message)
    {
        update(message.timestamp, { message.x, message.y, message.z });
    }));
}

HighGAccelerometerGraphWindow::~HighGAccelerometerGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
