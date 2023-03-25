import os
import sys
from dataclasses import dataclass

sys.path.append(os.path.join("..", "..", "..", ".."))  # location of helpers.py

import helpers


@dataclass
class Window:
    name: str
    callback_declarations: str
    y_axis: str
    legend: str
    callback_implementations: str
    return_window: str


windows = [
    Window(
        name="Gyroscope",
        callback_declarations="    std::function<void(ximu3::XIMU3_InertialMessage)> inertialCallback;",
        y_axis="\"Angular velocity (\" + degreeSymbol + \"/s)\"",
        legend="{{ \"X\", UIColours::graphRed },\n\
                                                                                                                  { \"Y\", UIColours::graphGreen },\n\
                                                                                                                  { \"Z\", UIColours::graphBlue }}",
        callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection().addInertialCallback(inertialCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.gyroscope_x, message.gyroscope_y, message.gyroscope_z });\n\
    }));",
        return_window="\
    if (type == WindowIDs::Gyroscope)\n\
    {\n\
        return window = std::make_shared<GyroscopeGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);\n\
    }\n",
    ),

    Window(
        name="EulerAngles",
        callback_declarations="\
    std::function<void(ximu3::XIMU3_QuaternionMessage)> quaternionCallback;\n\
    std::function<void(ximu3::XIMU3_RotationMatrixMessage)> rotationMatrixCallback;\n\
    std::function<void(ximu3::XIMU3_EulerAnglesMessage)> eulerAnglesCallback;\n\
    std::function<void(ximu3::XIMU3_LinearAccelerationMessage)> linearAccelerationCallback;\n\
    std::function<void(ximu3::XIMU3_EarthAccelerationMessage)> earthAccelerationCallback;",
        y_axis="\"Angle (\" + degreeSymbol + \")\"",
        legend="{{ \"Roll\",  UIColours::graphRed },\n\
                                                                                                     { \"Pitch\", UIColours::graphGreen },\n\
                                                                                                     { \"Yaw\",   UIColours::graphBlue }}",
        callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection().addQuaternionCallback(quaternionCallback = [&](auto message)\n\
    {\n\
        const auto eulerAngles = Helpers::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);\n\
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });\n\
    }));\n\
\n\
    callbackIDs.push_back(devicePanel.getConnection().addRotationMatrixCallback(rotationMatrixCallback = [&](auto message)\n\
    {\n\
        const auto quaternion = Helpers::toQuaternion(message.xx_element, message.xy_element, message.xz_element,\n\
                                                      message.yx_element, message.yy_element, message.yz_element,\n\
                                                      message.zx_element, message.zy_element, message.zz_element);\n\
        const auto eulerAngles = Helpers::toEulerAngles(quaternion.vector.x, quaternion.vector.y, quaternion.vector.z, quaternion.scalar);\n\
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });\n\
    }));\n\
\n\
    callbackIDs.push_back(devicePanel.getConnection().addEulerAnglesCallback(eulerAnglesCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { message.roll, message.pitch, message.yaw });\n\
    }));\n\
\n\
    callbackIDs.push_back(devicePanel.getConnection().addLinearAccelerationCallback(linearAccelerationCallback = [&](auto message)\n\
    {\n\
        const auto eulerAngles = Helpers::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);\n\
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });\n\
    }));\n\
\n\
    callbackIDs.push_back(devicePanel.getConnection().addEarthAccelerationCallback(earthAccelerationCallback = [&](auto message)\n\
    {\n\
        const auto eulerAngles = Helpers::toEulerAngles(message.x_element, message.y_element, message.z_element, message.w_element);\n\
        update(message.timestamp, { eulerAngles.x, eulerAngles.y, eulerAngles.z });\n\
    }));",
        return_window="\
    if (type == WindowIDs::EulerAngles)\n\
    {\n\
        return window = std::make_shared<EulerAnglesGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);\n\
    }\n",
    ),

    Window(
        name="ReceivedMessageRate",
        callback_declarations="    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;",
        y_axis="\"Receive rate (messages/s)\"",
        legend="{{{}, juce::Colours::yellow }}",
        callback_implementations="\
    callbackIDs.push_back(devicePanel.getConnection().addStatisticsCallback(statisticsCallback = [&](auto message)\n\
    {\n\
        update(message.timestamp, { (float) message.message_rate });\n\
    }));",
        return_window="\
    if (type == WindowIDs::ReceivedMessageRate)\n\
    {\n\
        return window = std::make_shared<ReceivedMessageRateGraphWindow>(windowLayout, windowTree.getType(), *this, glRenderer);\n\
    }\n",
    ),
]

# Generate *GraphWindow.h and *GraphWindow.cpp
for window in windows:
    with open("template_graph_window_h.txt") as file:
        template_h = file.read()

    with open("template_graph_window_cpp.txt") as file:
        template_cpp = file.read()

    template_h = template_h.replace("$name$", window.name)
    template_h = template_h.replace("$callback_declarations$", window.callback_declarations)

    template_cpp = template_cpp.replace("$name$", window.name)
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

for window in windows:
    code += window.return_window

helpers.insert(file_path, code, "1")
