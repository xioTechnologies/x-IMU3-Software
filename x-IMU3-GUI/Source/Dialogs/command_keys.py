import json
import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

with open("../DeviceSettings/DeviceSettings.json") as file:
    deviceSettings = [helpers.camel_case(j["name"]) for j in json.load(file)]

with open("CommandKeys.xml", "w") as file:
    file.write("<CommandKeys>\n")

    file.write("    <Command key=\"default\" type=\"4\"/>\n")
    file.write("    <Command key=\"apply\" type=\"4\"/>\n")
    file.write("    <Command key=\"save\" type=\"4\"/>\n")
    file.write("    <Command key=\"time\" type=\"0\"/>\n")
    file.write("    <Command key=\"ping\" type=\"4\"/>\n")
    file.write("    <Command key=\"reset\" type=\"4\"/>\n")
    file.write("    <Command key=\"shutdown\" type=\"4\"/>\n")
    file.write("    <Command key=\"strobe\" type=\"4\"/>\n")
    file.write("    <Command key=\"colour\" type=\"0\"/>\n")
    file.write("    <Command key=\"initialise\" type=\"4\"/>\n")
    file.write("    <Command key=\"heading\" type=\"1\"/>\n")
    file.write("    <Command key=\"accessory\" type=\"0\"/>\n")
    file.write("    <Command key=\"note\" type=\"0\"/>\n")
    file.write("    <Command key=\"format\" type=\"4\"/>\n")
    file.write("    <Command key=\"test\" type=\"4\"/>\n")
    file.write("    <Command key=\"bootloader\" type=\"4\"/>\n")

    file.write("    <Separator header=\"DEVICE SETTINGS\" type=\"4\"/>\n")

    for deviceSetting in deviceSettings:
        file.write("    <Command key=\"" + deviceSetting + "\" type=\"4\"/>\n")

    file.write("</CommandKeys>\n")
