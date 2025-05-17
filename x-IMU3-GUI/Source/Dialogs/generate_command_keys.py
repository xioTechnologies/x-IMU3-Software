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

with open("../DeviceSettings/DeviceSettings.json") as file:
    for setting in json.load(file):
        if bool(setting.get("calibration")):
            continue

        settings_keys.append(helpers.snake_case(setting["name"]))

        if any([d in setting["declaration"] for d in ["char name[", "RS9116BDAddress", "RS9116IPAddress", "RS9116LinkKey", "RS9116MacAddress"]]):
            settings_types.append(STRING)
        elif any([d in setting["declaration"] for d in ["float", "uint32_t", "int32_t", "uint16_t"]]):
            settings_types.append(NUMBER)
        elif "bool" in setting["declaration"]:
            settings_types.append(TRUE)
        else:
            json_type = setting["declaration"].split()[0]
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
    file.write(f'    <Command key="format" type="{NULL}"/>\n')
    file.write(f'    <Command key="test" type="{NULL}"/>\n')
    file.write(f'    <Command key="bootloader" type="{NULL}"/>\n')

    file.write('    <Separator header="DEVICE SETTINGS"/>\n')

    for device_settings_key, device_settings_type in zip(settings_keys, settings_types):
        file.write(f'    <Command key="{device_settings_key}" type="{device_settings_type}"/>\n')

    file.write("</CommandKeys>\n")
