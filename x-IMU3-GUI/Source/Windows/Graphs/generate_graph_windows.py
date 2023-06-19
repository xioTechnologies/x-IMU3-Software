import os
import sys
from dataclasses import dataclass

sys.path.append(os.path.join("..", "..", "..", ".."))  # location of helpers.py

import helpers


@dataclass
class Window:
    name: str
    callback_declarations: str
    horizontal_autoscale: str
    y_axis: str
    legend: str
    callback_implementations: str


windows = [
    Window(
        name="Gyroscope",
        callback_declarations="    std::function<void(ximu3::XIMU3_InertialMessage)> inertialCallback;",
        horizontal_autoscale="false",
        y_axis="Angular velocity (\" + degreeSymbol + \"/s)",
        legend="{{ \"X\", UIColours::graphRed },\n\
                                                                                                                   { \"Y\", UIColours::graphGreen },\n\
                                                                                                                   { \"Z\", UIColours::graphBlue }}",
        callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addInertialCallback(inertialCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.gyroscope_x, message.gyroscope_y, message.gyroscope_z });\n\
    }));",
    ),
    Window("Accelerometer",
           callback_declarations="    std::function<void(ximu3::XIMU3_InertialMessage)> inertialCallback;",
           horizontal_autoscale="false",
           y_axis="Acceleration (g)",
           legend="{{ \"X\", UIColours::graphRed },\n\
                                                                                          { \"Y\", UIColours::graphGreen },\n\
                                                                                          { \"Z\", UIColours::graphBlue }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addInertialCallback(inertialCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.accelerometer_x, message.accelerometer_y, message.accelerometer_z });\n\
    }));",
           ),
    Window("Magnetometer",
           callback_declarations="    std::function<void(ximu3::XIMU3_MagnetometerMessage)> magnetometerCallback;",
           horizontal_autoscale="false",
           y_axis="Intensity (a.u.)",
           legend="{{ \"X\", UIColours::graphRed },\n\
                                                                                          { \"Y\", UIColours::graphGreen },\n\
                                                                                          { \"Z\", UIColours::graphBlue }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addMagnetometerCallback(magnetometerCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });\n\
    }));",
           ),
    Window(
        name="EulerAngles",
        callback_declarations="\
    std::function<void(ximu3::XIMU3_QuaternionMessage)> quaternionCallback;\n\
    std::function<void(ximu3::XIMU3_RotationMatrixMessage)> rotationMatrixCallback;\n\
    std::function<void(ximu3::XIMU3_EulerAnglesMessage)> eulerAnglesCallback;\n\
    std::function<void(ximu3::XIMU3_LinearAccelerationMessage)> linearAccelerationCallback;\n\
    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;",
        horizontal_autoscale="false",
        y_axis="Angle (\" + degreeSymbol + \")",
        legend="{{ \"Roll\",  UIColours::graphRed },\n\
                                                                                                      { \"Pitch\", UIColours::graphGreen },\n\
                                                                                                      { \"Yaw\",   UIColours::graphBlue }}",
        callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addQuaternionCallback(quaternionCallback = [&](auto message)\n\
    {\n\
        const auto eulerAngles = Convert::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);\n\
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });\n\
    }));\n\
\n\
    callbackIDs.push_back(devicePanel.getConnection()->addRotationMatrixCallback(rotationMatrixCallback = [&](auto message)\n\
    {\n\
        const auto quaternion = Convert::toQuaternion(message.xx_element, message.xy_element, message.xz_element,\n\
                                                      message.yx_element, message.yy_element, message.yz_element,\n\
                                                      message.zx_element, message.zy_element, message.zz_element);\n\
        const auto eulerAngles = Convert::toEulerAngles(quaternion.vector.x, quaternion.vector.y, quaternion.vector.z, quaternion.scalar);\n\
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });\n\
    }));\n\
\n\
    callbackIDs.push_back(devicePanel.getConnection()->addEulerAnglesCallback(eulerAnglesCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.roll, message.pitch, message.yaw });\n\
    }));\n\
\n\
    callbackIDs.push_back(devicePanel.getConnection()->addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)\n\
    {\n\
        const auto eulerAngles = Convert::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);\n\
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });\n\
    }));\n\
\n\
    callbackIDs.push_back(devicePanel.getConnection()->addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)\n\
    {\n\
        const auto eulerAngles = Convert::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);\n\
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });\n\
    }));",
    ),
    Window("LinearAcceleration",
           callback_declarations="    std::function<void(ximu3::XIMU3_LinearAccelerationMessage)> linearAccelerationCallback;",
           horizontal_autoscale="false",
           y_axis="Acceleration (g)",
           legend="{{ \"X\", UIColours::graphRed },\n\
                                                                                          { \"Y\", UIColours::graphGreen },\n\
                                                                                          { \"Z\", UIColours::graphBlue }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });\n\
    }));",
           ),
    Window("EarthAcceleration",
           callback_declarations="    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;",
           horizontal_autoscale="false",
           y_axis="Acceleration (g)",
           legend="{{ \"X\", UIColours::graphRed },\n\
                                                                                          { \"Y\", UIColours::graphGreen },\n\
                                                                                          { \"Z\", UIColours::graphBlue }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });\n\
    }));",
           ),
    Window("HighGAccelerometer",
           callback_declarations="    std::function<void(ximu3::XIMU3_HighGAccelerometerMessage)> highGAccelerometerCallback;",
           horizontal_autoscale="false",
           y_axis="Acceleration (g)",
           legend="{{ \"X\", UIColours::graphRed },\n\
                                                                                          { \"Y\", UIColours::graphGreen },\n\
                                                                                          { \"Z\", UIColours::graphBlue }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addHighGAccelerometerCallback(highGAccelerometerCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.x_axis, message.y_axis, message.z_axis });\n\
    }));",
           ),
    Window("Temperature",
           callback_declarations="    std::function<void(ximu3::XIMU3_TemperatureMessage)> temperatureCallback;",
           horizontal_autoscale="true",
           y_axis="Temperature (\" + degreeSymbol + \"C)",
           legend="{{{}, juce::Colours::yellow }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addTemperatureCallback(temperatureCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.temperature });\n\
    }));",
           ),
    Window("BatteryPercentage",
           callback_declarations="    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;",
           horizontal_autoscale="true",
           y_axis="Percentage (%)",
           legend="{{{}, juce::Colours::yellow }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.percentage });\n\
    }));",
           ),
    Window("BatteryVoltage",
           callback_declarations="    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;",
           horizontal_autoscale="true",
           y_axis="Voltage (V)",
           legend="{{{}, juce::Colours::yellow }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.voltage });\n\
    }));",
           ),
    Window("RssiPercentage",
           callback_declarations="    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;",
           horizontal_autoscale="false",
           y_axis="Percentage (%)",
           legend="{{{}, juce::Colours::yellow }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.percentage });\n\
    }));",
           ),
    Window("RssiPower",
           callback_declarations="    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;",
           horizontal_autoscale="true",
           y_axis="Power (dBm)",
           legend="{{{}, juce::Colours::yellow }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.power });\n\
    }));",
           ),
    Window(
        name="ReceivedMessageRate",
        callback_declarations="    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;",
        horizontal_autoscale="true",
        y_axis="Receive rate (messages/s)",
        legend="{{{}, juce::Colours::yellow }}",
        callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addStatisticsCallback(statisticsCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { (float) message.message_rate });\n\
    }));",
    ),
    Window("ReceivedDataRate",
           callback_declarations="    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;",
           horizontal_autoscale="true",
           y_axis="Receive rate (kB/s)",
           legend="{{{}, juce::Colours::yellow }}",
           callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection()->addStatisticsCallback(statisticsCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { (float) message.data_rate / 1000.0f });\n\
    }));",
           ),
]

# Generate *GraphWindow.h and *GraphWindow.cpp
for window in windows:
    with open("template_h.txt") as file:
        template_h = file.read()

    with open("template_cpp.txt") as file:
        template_cpp = file.read()

    template_h = template_h.replace("$name$", window.name)
    template_h = template_h.replace("$callback_declarations$", window.callback_declarations)

    template_cpp = template_cpp.replace("$name$", window.name)
    template_cpp = template_cpp.replace("$horizontal_autoscale$", window.horizontal_autoscale)
    template_cpp = template_cpp.replace("$y_axis$", window.y_axis)
    template_cpp = template_cpp.replace("$legend$", window.legend)
    template_cpp = template_cpp.replace("$callback_implementations$", window.callback_implementations)

    with open(window.name + "GraphWindow.h", "w") as file:
        file.write(helpers.preamble())
        file.write(template_h)

    with open(window.name + "GraphWindow.cpp", "w") as file:
        file.write(helpers.preamble())
        file.write(template_cpp)

# Insert code into x-IMU3-GUI/Source/DevicePanel/DevicePanel.cpp
file_path = "../../DevicePanel/DevicePanel.cpp"

code = ""

for window in windows:
    code += "#include \"Windows/Graphs/" + window.name + "GraphWindow.h\"\n"

helpers.insert(file_path, code, "0")

code = ""

template = "\
    if (type == WindowIDs::$name$)\n\
    {\n\
        return window = std::make_shared<$name$GraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);\n\
    }\n"

for window in windows:
    code += template.replace("$name$", window.name)

helpers.insert(file_path, code, 1)
