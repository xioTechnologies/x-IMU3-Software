import re
from pathlib import Path

version = "1.9.1"


def replace(path, string):
    path.write_text(re.sub(string, string.replace("\\", "").replace(".*", version), path.read_text()))


for path in Path(__file__).resolve().parent.rglob("*"):
    if path.name == "Cargo.toml":
        replace(path, 'version = ".*"\n')

    if path.name == "pyproject.toml":
        replace(path, 'version = ".*"')

    if path.name == "CMakeLists.txt":
        replace(path, "project\\(x-IMU3-Software VERSION .*\\)")

    if path.name == "WindowsInstaller.iss":
        replace(path, "AppVersion=.*\n")
