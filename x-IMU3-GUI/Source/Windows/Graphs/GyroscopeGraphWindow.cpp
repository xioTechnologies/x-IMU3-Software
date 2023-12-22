// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "Convert.h"
#include "GyroscopeGraphWindow.h"

GyroscopeGraphWindow::GyroscopeGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, connectionPanel_,
                      glRenderer,
                      "Angular Rate (" + degreeSymbol + "/s)",
                      { "X", "Y", "Z" },
                      { UIColours::graphX, UIColours::graphY, UIColours::graphZ },
                      false)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addInertialCallback(inertialCallback = [&](auto message)
    {
        update(message.timestamp, { message.gyroscope_x, message.gyroscope_y, message.gyroscope_z });
    }));
}

GyroscopeGraphWindow::~GyroscopeGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
