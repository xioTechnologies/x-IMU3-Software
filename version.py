import os
import re

version = "1.7.1"


def replace(file_path, string):
    with open(file_path) as file:
        lines = file.readlines()

    with open(file_path, "w") as file:
        for line in lines:
            file.write(re.sub(string, string.replace("\\", "").replace(".*", version), line))


for root, _, files in os.walk(os.path.dirname(os.path.realpath(__file__))):
    for file in files:
        file_path = os.path.join(root, file)

        if file == "Cargo.toml":
            replace(file_path, 'version = ".*"\n')

        if file == "pyproject.toml":
            replace(file_path, 'version = ".*"')

        if file == "CMakeLists.txt":
            replace(file_path, "project\\(x-IMU3-Software VERSION .*\\)")

        if file == "WindowsInstaller.iss":
            replace(file_path, "AppVersion=.*\n")
