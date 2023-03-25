import json
import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers


with open("../DeviceSettings/DeviceSettings.json") as file:
    deviceSettings = [helpers.camel_case(j["name"]) for j in json.load(file)]

with open("CommandKeys.xml", "w") as file:
    file.write("<CommandKeys>\n")

    file.write("    <Command key=\"default\"/>\n")
    file.write("    <Command key=\"apply\"/>\n")
    file.write("    <Command key=\"save\"/>\n")
    file.write("    <Command key=\"time\"/>\n")
    file.write("    <Command key=\"ping\"/>\n")
    file.write("    <Command key=\"reset\"/>\n")
    file.write("    <Command key=\"shutdown\"/>\n")
    file.write("    <Command key=\"strobe\"/>\n")
    file.write("    <Command key=\"colour\"/>\n")
    file.write("    <Command key=\"color\"/>\n")
    file.write("    <Command key=\"heading\"/>\n")
    file.write("    <Command key=\"accessory\"/>\n")
    file.write("    <Command key=\"note\"/>\n")
    file.write("    <Command key=\"format\"/>\n")
    file.write("    <Command key=\"test\"/>\n")
    file.write("    <Command key=\"bootloader\"/>\n")

    file.write("    <Separator header=\"DEVICE SETTINGS\"/>\n")

    for deviceSetting in deviceSettings:
        file.write("    <Command key=\"" + deviceSetting + "\"/>\n")

    file.write("</CommandKeys>\n")
