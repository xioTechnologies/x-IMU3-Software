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
    legend_strings: str
    legend_colours: str
    callback_implementations: str


windows = [
    Window(
        name="Gyroscope",
        callback_declarations="    std::function<void(ximu3::XIMU3_InertialMessage)> inertialCallback;",
        horizontal_autoscale="false",
        y_axis='Angular Rate (" + degreeSymbol + "/s)',
        legend_strings='{ "X", "Y", "Z" }',
        legend_colours="{ UIColours::graphX, UIColours::graphY, UIColours::graphZ }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addInertialCallback(inertialCallback = [&](auto message)
    {
        update(message.timestamp, { message.gyroscope_x, message.gyroscope_y, message.gyroscope_z });
    }));""",
    ),
    Window(
        name="Accelerometer",
        callback_declarations="    std::function<void(ximu3::XIMU3_InertialMessage)> inertialCallback;",
        horizontal_autoscale="false",
        y_axis="Acceleration (g)",
        legend_strings='{ "X", "Y", "Z" }',
        legend_colours="{ UIColours::graphX, UIColours::graphY, UIColours::graphZ }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addInertialCallback(inertialCallback = [&](auto message)
    {
        update(message.timestamp, { message.accelerometer_x, message.accelerometer_y, message.accelerometer_z });
    }));""",
    ),
    Window(
        name="Magnetometer",
        callback_declarations="    std::function<void(ximu3::XIMU3_MagnetometerMessage)> magnetometerCallback;",
        horizontal_autoscale="false",
        y_axis="Intensity (a.u.)",
        legend_strings='{ "X", "Y", "Z" }',
        legend_colours="{ UIColours::graphX, UIColours::graphY, UIColours::graphZ }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addMagnetometerCallback(magnetometerCallback = [&](auto message)
    {
        update(message.timestamp, { message.x, message.y, message.z });
    }));""",
    ),
    Window(
        name="EulerAngles",
        callback_declarations="""\
    std::function<void(ximu3::XIMU3_QuaternionMessage)> quaternionCallback;
    std::function<void(ximu3::XIMU3_RotationMatrixMessage)> rotationMatrixCallback;
    std::function<void(ximu3::XIMU3_EulerAnglesMessage)> eulerAnglesCallback;
    std::function<void(ximu3::XIMU3_LinearAccelerationMessage)> linearAccelerationCallback;
    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;""",
        horizontal_autoscale="false",
        y_axis='Angle (" + degreeSymbol + ")',
        legend_strings='{ "Roll", "Pitch", "Yaw" }',
        legend_colours="{ UIColours::graphX, UIColours::graphY, UIColours::graphZ }",
        callback_implementations="""\
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
        update(message.timestamp, { eulerAngles.roll, eulerAngles.pitch, eulerAngles.yaw });\n\
    }));""",
    ),
    Window(
        name="LinearAcceleration",
        callback_declarations="    std::function<void(ximu3::XIMU3_LinearAccelerationMessage)> linearAccelerationCallback;",
        horizontal_autoscale="false",
        y_axis="Acceleration (g)",
        legend_strings='{ "X", "Y", "Z" }',
        legend_colours="{ UIColours::graphX, UIColours::graphY, UIColours::graphZ }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)
    {
        update(message.timestamp, { message.acceleration_x, message.acceleration_y, message.acceleration_z });
    }));""",
    ),
    Window(
        name="EarthAcceleration",
        callback_declarations="    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;",
        horizontal_autoscale="false",
        y_axis="Acceleration (g)",
        legend_strings='{ "X", "Y", "Z" }',
        legend_colours="{ UIColours::graphX, UIColours::graphY, UIColours::graphZ }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)
    {
        update(message.timestamp, { message.acceleration_x, message.acceleration_y, message.acceleration_z });
    }));""",
    ),
    Window(
        name="HighGAccelerometer",
        callback_declarations="    std::function<void(ximu3::XIMU3_HighGAccelerometerMessage)> highGAccelerometerCallback;",
        horizontal_autoscale="false",
        y_axis="Acceleration (g)",
        legend_strings='{ "X", "Y", "Z" }',
        legend_colours="{ UIColours::graphX, UIColours::graphY, UIColours::graphZ }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addHighGAccelerometerCallback(highGAccelerometerCallback = [&](auto message)
    {
        update(message.timestamp, { message.x, message.y, message.z });
    }));""",
    ),
    Window(
        name="Temperature",
        callback_declarations="    std::function<void(ximu3::XIMU3_TemperatureMessage)> temperatureCallback;",
        horizontal_autoscale="true",
        y_axis='Temperature (" + degreeSymbol + "C)',
        legend_strings='{ "" }',
        legend_colours="{ UIColours::graphChannel1 }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addTemperatureCallback(temperatureCallback = [&](auto message)
    {
        update(message.timestamp, { message.temperature });
    }));""",
    ),
    Window(
        name="BatteryPercentage",
        callback_declarations="    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;",
        horizontal_autoscale="true",
        y_axis="Percentage (%)",
        legend_strings='{ "" }',
        legend_colours="{ UIColours::graphChannel1 }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)
    {
        update(message.timestamp, { message.percentage });
    }));""",
    ),
    Window(
        name="BatteryVoltage",
        callback_declarations="    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;",
        horizontal_autoscale="true",
        y_axis="Voltage (V)",
        legend_strings='{ "" }',
        legend_colours="{ UIColours::graphChannel1 }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)
    {
        update(message.timestamp, { message.voltage });
    }));""",
    ),
    Window(
        name="RssiPercentage",
        callback_declarations="    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;",
        horizontal_autoscale="true",
        y_axis="Percentage (%)",
        legend_strings='{ "" }',
        legend_colours="{ UIColours::graphChannel1 }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)
    {
        update(message.timestamp, { message.percentage });
    }));""",
    ),
    Window(
        name="RssiPower",
        callback_declarations="    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;",
        horizontal_autoscale="true",
        y_axis="Power (dBm)",
        legend_strings='{ "" }',
        legend_colours="{ UIColours::graphChannel1 }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)
    {
        update(message.timestamp, { message.power });
    }));""",
    ),
    Window(
        name="SerialAccessoryCsvs",
        callback_declarations="    std::function<void(ximu3::XIMU3_SerialAccessoryMessage)> serialAccessoryCallback;",
        horizontal_autoscale="false",
        y_axis="CSV",
        legend_strings='{ "1", "2", "3", "4", "5", "6", "7", "8" }',
        legend_colours="{ UIColours::graphChannel1, UIColours::graphChannel2, UIColours::graphChannel3, UIColours::graphChannel4, UIColours::graphChannel5, UIColours::graphChannel6, UIColours::graphChannel7, UIColours::graphChannel8 }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addSerialAccessoryCallback(serialAccessoryCallback = [&](auto message)
    {
        std::vector<float> values;
        for (const auto& string : juce::StringArray::fromTokens(juce::String::createStringFromData(message.char_array, (int) message.number_of_bytes), ",", ""))
        {
            values.push_back(string.getFloatValue());
        }
        update(message.timestamp, values);
    }));""",
    ),
    Window(
        name="ReceivedMessageRate",
        callback_declarations="    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;",
        horizontal_autoscale="true",
        y_axis="Throughput (messages/s)",
        legend_strings='{ "" }',
        legend_colours="{ UIColours::graphChannel1 }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addStatisticsCallback(statisticsCallback = [&](auto message)
    {
        update(message.timestamp, { (float) message.message_rate });
    }));""",
    ),
    Window(
        name="ReceivedDataRate",
        callback_declarations="    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;",
        horizontal_autoscale="true",
        y_axis="Throughput (kB/s)",
        legend_strings='{ "" }',
        legend_colours="{ UIColours::graphChannel1 }",
        callback_implementations="""\
    callbackIDs.push_back(connectionPanel.getConnection()->addStatisticsCallback(statisticsCallback = [&](auto message)
    {
        update(message.timestamp, { (float) message.data_rate / 1000.0f });
    }));""",
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
    template_cpp = template_cpp.replace("$legend_strings$", window.legend_strings)
    template_cpp = template_cpp.replace("$legend_colours$", window.legend_colours)
    template_cpp = template_cpp.replace("$callback_implementations$", window.callback_implementations)

    with open(window.name + "GraphWindow.h", "w") as file:
        file.write(helpers.preamble())
        file.write(template_h)

    with open(window.name + "GraphWindow.cpp", "w") as file:
        file.write(helpers.preamble())
        file.write(template_cpp)

# Insert code into x-IMU3-GUI/Source/ConnectionPanel/ConnectionPanel.cpp
file_path = "../../ConnectionPanel/ConnectionPanel.cpp"

code = "".join(['#include "Windows/Graphs/' + w.name + 'GraphWindow.h"\n' for w in windows])
helpers.insert(file_path, code, "0")

template = """\
    if (type == WindowIDs::$name$)
    {
        return window = std::make_shared<$name$GraphWindow>(windowLayout, type, *this, glRenderer);
    }\n"""

code = "".join([template.replace("$name$", w.name) for w in windows])
helpers.insert(file_path, code, 1)
