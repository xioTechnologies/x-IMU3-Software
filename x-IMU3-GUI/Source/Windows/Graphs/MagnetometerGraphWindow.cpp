// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "../../Convert.h"
#include "../../DevicePanel/DevicePanel.h"
#include "MagnetometerGraphWindow.h"

juce::ValueTree MagnetometerGraphWindow::settingsTree_("MagnetometerGraphSettings");

MagnetometerGraphWindow::MagnetometerGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, devicePanel_,
                      glRenderer,
                      "Intensity (a.u.)",
                      { "X", "Y", "Z" },
                      { UIColours::graphX, UIColours::graphY, UIColours::graphZ },
                      settingsTree_,
                      false)
{
    callbackIDs.push_back(devicePanel.getConnection()->addMagnetometerCallback(magnetometerCallback = [&](auto message)
    {
        write(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });
    }));
}

MagnetometerGraphWindow::~MagnetometerGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection()->removeCallback(callbackID);
    }
}
