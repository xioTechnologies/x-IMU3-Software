#include "GyroscopeGraphWindow.h"

Graph::Settings GyroscopeGraphWindow::settings;

GyroscopeGraphWindow::GyroscopeGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type, devicePanel_, glRenderer, "Angular velocity (" + degreeSymbol + "/s)", {{ "X", UIColours::graphRed },
                                                                                                                  { "Y", UIColours::graphGreen },
                                                                                                                  { "Z", UIColours::graphBlue }}, settings)
{
    callbackIDs = { devicePanel.getConnection().addInertialCallback(inertialCallback = [&](auto message)
    {
        update(message.timestamp, { message.gyroscope_x, message.gyroscope_y, message.gyroscope_z });
    }) };
}

GyroscopeGraphWindow::~GyroscopeGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection().removeCallback(callbackID);
    }
}
