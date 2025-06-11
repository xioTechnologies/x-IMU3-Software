import os
import sys
from itertools import dropwhile, takewhile

sys.path.append(os.path.join("..", ".."))  # location of helpers.py

import helpers


def extract_words(line, start_token, end_token):
    # if start_token not in string:
    #     raise ValueError(f'start_token "{start_token}" not in string "{string}"')

    # string = f"{start_token}".join(string.split(start_token)[1:])

    # if end_token not in string:
    #     raise ValueError(f'end_token "{end_token}" not in string "{string}"')

    # return string.split(end_token)[0]
    return line.split(start_token)[1].split(end_token)[0]


def extract_lines(lines, start_token, end_token):
    pass


# Prase Python-C-API/ximu3.c
with open("Python-C-API/ximu3.c") as file:
    lines = file.readlines()

    constants = [extract_words(l, '"', '"') for l in lines if "PyModule_AddIntConstant" in l]

    functions_arrays = [extract_words(l, ", ", ")") for l in lines if "PyModule_AddFunctions" in l]

    objects = [extract_words(l, "&", ",") for l in lines if "add_object(module" in l]

# Load .h files
header_files = []

for root, _, file_names in os.walk(os.path.dirname(os.path.realpath(__file__))):
    for file_name in file_names:
        _, extension = os.path.splitext(file_name)

        if extension != ".h":
            continue

        with open(os.path.join(root, file_name)) as file:
            header_files.append(file.readlines())

# Constants
constants = "\n".join([f"{c}: int" for c in constants])

# Functions
functions = []

for functions_array in functions_arrays:
    for lines in header_files:
        if not any(functions_array in l for l in lines):
            continue

        lines = list(dropwhile(lambda l: functions_array not in l, lines))[1:]  # discard first item
        lines = list(takewhile(lambda l: "{ NULL }" not in l, lines))

        for line in lines:
            functions.append(extract_words(line, '{ "', '",'))

functions = "\n".join(f"def {f}(): ..." for f in functions)

# Objects

# ...
with open("ximu3-stubs/__init__.pyi", "w") as file:
    file.write(f"""\
{helpers.preamble("#")}
{constants}

{functions}

class PortScanner:
    def __init__(self, callback): ...
    def get_devices(): ...
    def scan(): ...
    def scan_filter(connection_type): ...
    def get_port_names(): ...

class UsbConnectionInfo:
    port_name: ...
    def __init__(self, port_name): ...
    def to_string(): ...
""")
