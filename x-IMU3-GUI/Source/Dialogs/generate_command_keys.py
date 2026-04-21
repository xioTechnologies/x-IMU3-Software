import json
import sys
from pathlib import Path

sys.path.append(str(Path("../../..")))  # location of helpers.py

import helpers

STRING = "0"
NUMBER = "1"
TRUE = "2"
NULL = "4"

settings_keys = []
settings_types = []

for setting in json.loads(Path("../DeviceSettings/Settings.json").read_text())["settings"]:
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
        settings_types.append(NUMBER)

fixed_commands = "\n".join(
    f'    <Command key="{k}" type="{t}"/>'
    for k, t in [
        ("default", NULL),
        ("apply", NULL),
        ("save", NULL),
        ("time", STRING),
        ("ping", NULL),
        ("reset", NULL),
        ("shutdown", NULL),
        ("blink", NULL),
        ("strobe", NULL),
        ("colour", STRING),
        ("start", STRING),
        ("stop", NULL),
        ("delete", STRING),
        ("initialise", NULL),
        ("heading", NUMBER),
        ("accessory", STRING),
        ("note", STRING),
        ("timestamp", NUMBER),
        ("format", NULL),
        ("test", NULL),
        ("bootloader", NULL),
    ]
)

setting_commands = "\n".join(f'    <Command key="{k}" type="{t}"/>' for k, t in zip(settings_keys, settings_types))

Path("CommandKeys.xml").write_text(f"""\
<CommandKeys>
{fixed_commands}
    <Separator header="DEVICE SETTINGS"/>
{setting_commands}
</CommandKeys>
""")
