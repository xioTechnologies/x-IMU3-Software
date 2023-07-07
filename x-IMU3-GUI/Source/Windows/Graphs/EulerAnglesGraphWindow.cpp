// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "../../Convert.h"
#include "EulerAnglesGraphWindow.h"

Graph::Settings EulerAnglesGraphWindow::settings = Graph::Settings(false);

EulerAnglesGraphWindow::EulerAnglesGraphWindow(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, GLRenderer& glRenderer)
        : GraphWindow(windowLayout, type_, devicePanel_, glRenderer, "Angle (" + degreeSymbol + ")", {{ "Roll",  UIColours::graphRed },
                                                                                                      { "Pitch", UIColours::graphGreen },
                                                                                                      { "Yaw",   UIColours::graphBlue }}, settings, Graph::Settings(false))
{
    callbackIDs.push_back(devicePanel.getConnection()->addQuaternionCallback(quaternionCallback = [&](auto message)
    {
        const auto eulerAngles = Convert::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });
    }));

    callbackIDs.push_back(devicePanel.getConnection()->addRotationMatrixCallback(rotationMatrixCallback = [&](auto message)
    {
        const auto quaternion = Convert::toQuaternion(message.xx_element, message.xy_element, message.xz_element,
                                                      message.yx_element, message.yy_element, message.yz_element,
                                                      message.zx_element, message.zy_element, message.zz_element);
        const auto eulerAngles = Convert::toEulerAngles(quaternion.vector.x, quaternion.vector.y, quaternion.vector.z, quaternion.scalar);
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });
    }));

    callbackIDs.push_back(devicePanel.getConnection()->addEulerAnglesCallback(eulerAnglesCallback = [&](auto message)
    {
        update(message.timestamp, { message.roll, message.pitch, message.yaw });
    }));

    callbackIDs.push_back(devicePanel.getConnection()->addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)
    {
        const auto eulerAngles = Convert::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });
    }));

    callbackIDs.push_back(devicePanel.getConnection()->addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
    {
        const auto eulerAngles = Convert::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });
    }));
}

EulerAnglesGraphWindow::~EulerAnglesGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        devicePanel.getConnection()->removeCallback(callbackID);
    }
}
