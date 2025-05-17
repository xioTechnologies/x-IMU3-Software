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
    if any([d in setting["declaration"] for d in ["char name[", "RS9116BDAddress", "RS9116IPAddress", "RS9116LinkKey", "RS9116MacAddress"]]):
        json_type = "string"
    elif any([d in setting["declaration"] for d in ["float", "uint32_t", "int32_t", "uint16_t"]]):
        json_type = "number"
    elif "bool" in setting["declaration"]:
        json_type = "bool"
    else:
        json_type = setting["declaration"].split()[0]
        enum_types.append(json_type)

    json_types.append(json_type)

enum_types = list(set(enum_types))
enum_types.sort()

with open("DeviceSettings.xml", "w") as file:
    file.write("<DeviceSettings>\n")
    file.write("    <Settings>\n")

    for setting, json_type in zip(settings, json_types):
        file.write(f'        <Setting key="{helpers.snake_case(setting["name"])}" name="{helpers.title_case(setting["name"])}" type="{json_type}" {('readOnly="true"' if setting.get("read only") else "")}/>\n')

    file.write("        <Margin/>\n")
    file.write("    </Settings>\n")

    file.write("    <Enums>\n")

    for enum_type in enum_types:
        file.write(f"""\
        <Enum name="{enum_type}">
            <Enumerator name="Zero" value="0"/>
            <Enumerator name="One" value="1"/>
        </Enum>\n""")

    file.write("    </Enums>\n")

    file.write("</DeviceSettings>\n")
