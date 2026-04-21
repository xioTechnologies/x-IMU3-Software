import json
import sys
from pathlib import Path

sys.path.append(str(Path("../../..")))  # location of helpers.py

import helpers

settings = json.loads(Path("Settings.json").read_text())["settings"]

json_types = []
enum_types = []

for setting in settings:
    if any(d in setting["declaration"] for d in ["char name[", "RS9116BDAddress", "RS9116IPAddress", "RS9116LinkKey", "RS9116MacAddress"]):
        json_type = "string"
    elif any(d in setting["declaration"] for d in ["float", "uint32_t", "int32_t", "uint16_t"]):
        json_type = "number"
    elif "bool" in setting["declaration"]:
        json_type = "bool"
    else:
        json_type = setting["declaration"].split()[0]
        enum_types.append(json_type)

    json_types.append(json_type)

enum_types = sorted(set(enum_types))

setting_elements = "\n".join(f'        <Setting key="{helpers.snake_case(s["name"])}" name="{helpers.title_case(s["name"])}" type="{t}" {('readOnly="true"' if s.get("read-only") else "")}/>' for s, t in zip(settings, json_types))

enum_elements = "\n".join(
    f"""\
        <Enum name="{e}">
            <Enumerator name="Zero" value="0"/>
            <Enumerator name="One" value="1"/>
        </Enum>"""
    for e in enum_types
)

Path("DeviceSettings.xml").write_text(f"""\
<DeviceSettings>
    <Settings>
{setting_elements}
        <Margin/>
    </Settings>
    <Enums>
{enum_elements}
    </Enums>
</DeviceSettings>
""")
