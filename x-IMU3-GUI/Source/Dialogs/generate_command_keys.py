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
        ("ping", NULL),
        ("default", NULL),
        ("apply", NULL),
        ("save", NULL),
        ("time", STRING),
        ("format", NULL),
        ("capacity", NULL),
        ("start", STRING),
        ("stop", NULL),
        ("delete", STRING),
        ("restart", NULL),
        ("heading", NUMBER),
        ("anchor_start", NUMBER),
        ("anchor_progress", NULL),
        ("anchor_complete", NULL),
        ("anchor_abort", NULL),
        ("bias_start", NUMBER),
        ("bias_progress", NULL),
        ("bias_complete", NULL),
        ("bias_abort", NULL),
        ("hard_iron_start", NUMBER),
        ("hard_iron_progress", NULL),
        ("hard_iron_complete", NULL),
        ("hard_iron_abort", NULL),
        ("accessory", STRING),
        ("note", STRING),
        ("timestamp", NUMBER),
        ("blink", NULL),
        ("strobe", NULL),
        ("colour", STRING),
        ("reset", NULL),
        ("shutdown", NULL),
        ("bootloader", NULL),
        ("test", NULL),
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
