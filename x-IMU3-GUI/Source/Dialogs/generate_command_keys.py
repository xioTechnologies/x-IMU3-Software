import json
import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

STRING = "0"
NUMBER = "1"
TRUE = "2"
NULL = "4"

device_settings_keys = []
device_settings_types = []

with open("../DeviceSettings/DeviceSettings.json") as file:
    for setting in json.load(file):
        if bool(setting.get("calibration")):
            continue

        device_settings_keys.append(helpers.camel_case(setting["name"]))

        if any([d in setting["declaration"] for d in ["char name[", "RS9116BDAddress", "RS9116IPAddress", "RS9116LinkKey", "RS9116MacAddress"]]):
            device_settings_types.append(STRING)
        elif any([d in setting["declaration"] for d in ["float", "uint32_t", "int32_t", "uint16_t"]]):
            device_settings_types.append(NUMBER)
        elif "bool" in setting["declaration"]:
            device_settings_types.append(TRUE)
        else:
            json_type = setting["declaration"].split()[0]
            device_settings_types.append(NUMBER)

with open("CommandKeys.xml", "w") as file:
    file.write("<CommandKeys>\n")

    file.write("    <Command key=\"default\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"apply\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"save\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"time\" type=\"" + STRING + "\"/>\n")
    file.write("    <Command key=\"ping\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"reset\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"shutdown\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"blink\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"strobe\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"colour\" type=\"" + STRING + "\"/>\n")
    file.write("    <Command key=\"start\" type=\"" + STRING + "\"/>\n")
    file.write("    <Command key=\"stop\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"delete\" type=\"" + STRING + "\"/>\n")
    file.write("    <Command key=\"initialise\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"heading\" type=\"" + NUMBER + "\"/>\n")
    file.write("    <Command key=\"accessory\" type=\"" + STRING + "\"/>\n")
    file.write("    <Command key=\"note\" type=\"" + STRING + "\"/>\n")
    file.write("    <Command key=\"format\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"test\" type=\"" + NULL + "\"/>\n")
    file.write("    <Command key=\"bootloader\" type=\"" + NULL + "\"/>\n")

    file.write("    <Separator header=\"DEVICE SETTINGS\"/>\n")

    for device_settings_key, device_settings_type in zip(device_settings_keys, device_settings_types):
        file.write("    <Command key=\"" + device_settings_key + "\" type=\"" + device_settings_type + "\"/>\n")

    file.write("</CommandKeys>\n")
