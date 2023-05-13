import json
import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

with open("DeviceSettings.json") as file:
    settings = json.load(file)

json_types = []
enum_types = []

for setting in settings:
    json_type = ""

    for declaration in {"char name[", "RS9116BDAddress", "RS9116IPAddress", "RS9116LinkKey", "RS9116MacAddress"}:
        if declaration in setting["declaration"]:
            json_type = "string"

    for declaration in {"float", "uint32_t", "int32_t", "uint16_t"}:
        if declaration in setting["declaration"]:
            json_type = "number"

    if "bool" in setting["declaration"]:
        json_type = "bool"

    if json_type == "":
        json_type = setting["declaration"].split()[0]
        enum_types.append(json_type)

    json_types.append(json_type)

with open("DeviceSettings.xml", "w") as file:
    file.write("<DeviceSettings>\n")

    for setting, json_type in zip(settings, json_types):
        file.write("    <Setting " +
                   "key=\"" + helpers.camel_case(setting["name"]) + "\" " +
                   "name=\"" + helpers.title_case(setting["name"]) + "\" " +
                   "type=\"" + json_type + "\" " +
                   ("readOnly=\"" + "true" + "\"" if setting.get("read only") else "") + "/>\n")

    file.write("</DeviceSettings>\n")

enum_types = list(set(enum_types))  # remove duplicates
enum_types.sort()  # sort alphabetically

with open("DeviceSettingsEnums.xml", "w") as file:
    file.write("<DeviceSettingsEnums>\n")

    for enum_type in enum_types:
        file.write("\
    <Enum name=\"" + enum_type + "\">\n\
        <Option name=\"Zero\" value=\"0\"/>\n\
        <Option name=\"One\" value=\"1\"/>\n\
    </Enum>\n")

    file.write("</DeviceSettingsEnums>\n")
