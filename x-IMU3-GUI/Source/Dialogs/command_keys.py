import json
import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

with open("../DeviceSettings/DeviceSettings.json") as file:
    deviceSettings = [helpers.camel_case(j["name"]) for j in json.load(file)]

with open("CommandKeys.xml", "w") as file:
    file.write("<CommandKeys>\n")

    file.write("    <Command key=\"default\" type=\"null\"/>\n")
    file.write("    <Command key=\"apply\" type=\"null\"/>\n")
    file.write("    <Command key=\"save\" type=\"null\"/>\n")
    file.write("    <Command key=\"time\" type=\"string\"/>\n")
    file.write("    <Command key=\"ping\" type=\"null\"/>\n")
    file.write("    <Command key=\"reset\" type=\"null\"/>\n")
    file.write("    <Command key=\"shutdown\" type=\"null\"/>\n")
    file.write("    <Command key=\"strobe\" type=\"null\"/>\n")
    file.write("    <Command key=\"colour\" type=\"string\"/>\n")
    file.write("    <Command key=\"initialise\" type=\"null\"/>\n")
    file.write("    <Command key=\"heading\" type=\"number\"/>\n")
    file.write("    <Command key=\"accessory\" type=\"string\"/>\n")
    file.write("    <Command key=\"note\" type=\"string\"/>\n")
    file.write("    <Command key=\"format\" type=\"null\"/>\n")
    file.write("    <Command key=\"test\" type=\"null\"/>\n")
    file.write("    <Command key=\"bootloader\" type=\"null\"/>\n")

    file.write("    <Separator header=\"DEVICE SETTINGS\" type=\"null\"/>\n")

    for deviceSetting in deviceSettings:
        file.write("    <Command key=\"" + deviceSetting + "\" type=\"null\"/>\n")

    file.write("</CommandKeys>\n")
