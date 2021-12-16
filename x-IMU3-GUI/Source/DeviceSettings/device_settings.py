import json
import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

keys = []
display_names = []
json_types = []
read_onlys = []
enum_types = []

with open("DeviceSettings.json") as file:
    for json_object in json.load(file):
        keys.append(helpers.camel_case(json_object["name"]))

        display_names.append(helpers.title_case(json_object["name"]))

        json_type = ""

        for declaration in {"char name[", "RS9116BDAddress", "RS9116IPAddress", "RS9116LinkKey", "RS9116MacAddress"}:
            if declaration in json_object["declaration"]:
                json_type = "string"

        for declaration in {"float", "uint32_t", "int32_t", "uint16_t"}:
            if declaration in json_object["declaration"]:
                json_type = "number"

        if "bool" in json_object["declaration"]:
            json_type = "bool"

        if json_type == "":
            json_type = json_object["declaration"].split()[0]
            enum_types.append(json_type)

        json_types.append(json_type)

        try:
            read_only = json_object["read only"]
        except:
            read_only = False

        if read_only:
            read_onlys.append("true")
        else:
            read_onlys.append("false")

with open("DeviceSettings.xml", "w") as file:
    file.write("<Settings>\n")

    for index in range(len(keys)):
        file.write("    <Setting " +
                   "key=\"" + keys[index] + "\" " +
                   "displayName=\"" + display_names[index] + "\" " +
                   "type=\"" + json_types[index] + "\" " +
                   "readOnly=\"" + read_onlys[index] + "\"" +
                   "/>\n")

    file.write("</Settings>\n")

with open("DeviceSettingsTypes.xml", "w") as file:
    file.write("<Types>\n")

    enum_types = list(set(enum_types))
    enum_types.sort()

    for enum_type in enum_types:
        file.write("\
    <Enum typeName=\"" + enum_type + "\">\n\
        <Option displayName=\"Zero\" value=\"0\"/>\n\
        <Option displayName=\"One\" value=\"1\"/>\n\
    </Enum>\n")

    file.write("</Types>\n")
