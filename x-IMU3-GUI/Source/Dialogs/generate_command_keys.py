import json
import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

STRING = "0"
NUMBER = "1"
TRUE = "2"
NULL = "4"

settings_keys = []
settings_types = []

with open("../DeviceSettings/Settings.json") as file:
    for setting in json.load(file)["settings"]:
        name = helpers.snake_case(setting["name"])

        if name in (
            "calibration_date",
            "gyroscope_misalignment",
            "gyroscope_sensitivity",
            "gyroscope_offset",
            "accelerometer_misalignment",
            "accelerometer_sensitivity",
            "accelerometer_offset",
            "soft_iron_matrix",
            "hard_iron_offset",
            "high_g_accelerometer_misalignment",
            "high_g_accelerometer_sensitivity",
            "high_g_accelerometer_offset",
        ):
            continue

        settings_keys.append(name)

        declaration = setting["declaration"]

        if any(d in declaration for d in ("char name[", "RS9116BDAddress", "RS9116IPAddress", "RS9116LinkKey", "RS9116MacAddress")):
            settings_types.append(STRING)
        elif any(d in declaration for d in ("float", "uint32_t", "int32_t", "uint16_t")):
            settings_types.append(NUMBER)
        elif "bool" in declaration:
            settings_types.append(TRUE)
        else:
            json_type = declaration.split()[0]
            settings_types.append(NUMBER)

with open("CommandKeys.xml", "w") as file:
    file.write("<CommandKeys>\n")

    file.write(f'    <Command key="default" type="{NULL}"/>\n')
    file.write(f'    <Command key="apply" type="{NULL}"/>\n')
    file.write(f'    <Command key="save" type="{NULL}"/>\n')
    file.write(f'    <Command key="time" type="{STRING}"/>\n')
    file.write(f'    <Command key="ping" type="{NULL}"/>\n')
    file.write(f'    <Command key="reset" type="{NULL}"/>\n')
    file.write(f'    <Command key="shutdown" type="{NULL}"/>\n')
    file.write(f'    <Command key="blink" type="{NULL}"/>\n')
    file.write(f'    <Command key="strobe" type="{NULL}"/>\n')
    file.write(f'    <Command key="colour" type="{STRING}"/>\n')
    file.write(f'    <Command key="start" type="{STRING}"/>\n')
    file.write(f'    <Command key="stop" type="{NULL}"/>\n')
    file.write(f'    <Command key="delete" type="{STRING}"/>\n')
    file.write(f'    <Command key="initialise" type="{NULL}"/>\n')
    file.write(f'    <Command key="heading" type="{NUMBER}"/>\n')
    file.write(f'    <Command key="accessory" type="{STRING}"/>\n')
    file.write(f'    <Command key="note" type="{STRING}"/>\n')
    file.write(f'    <Command key="timestamp" type="{NUMBER}"/>\n')
    file.write(f'    <Command key="format" type="{NULL}"/>\n')
    file.write(f'    <Command key="test" type="{NULL}"/>\n')
    file.write(f'    <Command key="bootloader" type="{NULL}"/>\n')

    file.write('    <Separator header="DEVICE SETTINGS"/>\n')

    for device_settings_key, device_settings_type in zip(settings_keys, settings_types):
        file.write(f'    <Command key="{device_settings_key}" type="{device_settings_type}"/>\n')

    file.write("</CommandKeys>\n")
