// This file was generated by x-IMU3-GUI/Source/Windows/Graphs/generate_graph_windows.py

#include "ConnectionPanel/ConnectionPanel.h"
#include "EulerAnglesGraphWindow.h"

EulerAnglesGraphWindow::EulerAnglesGraphWindow(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, GLRenderer& glRenderer)
    : GraphWindow(windowLayout_, type_, connectionPanel_,
                  glRenderer,
                  "Angle (" + degreeSymbol + ")",
                  { "Roll", "Pitch", "Yaw" },
                  { UIColours::graphX, UIColours::graphY, UIColours::graphZ },
                  false)
{
    callbackIDs.push_back(connectionPanel.getConnection()->addQuaternionCallback(quaternionCallback = [&](auto message)
    {
        const auto eulerAngles = ximu3::XIMU3_quaternion_message_to_euler_angles_message(message);
        update(message.timestamp, { eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw });
    }));

    callbackIDs.push_back(connectionPanel.getConnection()->addRotationMatrixCallback(rotationMatrixCallback = [&](auto message)
    {
        const auto eulerAngles = ximu3::XIMU3_rotation_matrix_message_to_euler_angles_message(message);
        update(message.timestamp, { eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw });
    }));

    callbackIDs.push_back(connectionPanel.getConnection()->addEulerAnglesCallback(eulerAnglesCallback = [&](auto message)
    {
        update(message.timestamp, { message.roll, message.pitch, message.yaw });
    }));

    callbackIDs.push_back(connectionPanel.getConnection()->addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)
    {
        const auto eulerAngles = ximu3::XIMU3_linear_acceleration_message_to_euler_angles_message(message);
        update(message.timestamp, { eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw });
    }));

    callbackIDs.push_back(connectionPanel.getConnection()->addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
    {
        const auto eulerAngles = ximu3::XIMU3_earth_acceleration_message_to_euler_angles_message(message);
        update(message.timestamp, { eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw });
    }));
}

EulerAnglesGraphWindow::~EulerAnglesGraphWindow()
{
    for (const auto callbackID : callbackIDs)
    {
        connectionPanel.getConnection()->removeCallback(callbackID);
    }
}
