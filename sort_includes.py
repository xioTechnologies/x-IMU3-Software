import os
from itertools import dropwhile, takewhile

paths = []

for root, _, file_names in os.walk(os.path.dirname(os.path.realpath(__file__))):
    if "config" in root:
        continue

    for file_name in file_names:
        _, extension = os.path.splitext(file_name)

        if extension in (".h", ".c"):
            paths.append(os.path.join(root, file_name))


for path in paths:
    with open(path) as file:
        lines = file.readlines()

    lines = [l.rstrip() + "\n" for l in lines]

    include_lines = [l for l in lines if l.lstrip().startswith("#include")]

    lines_before_first_include = list(takewhile(lambda l: not l.lstrip().startswith("#include"), lines))

    lines_from_first_include = list(dropwhile(lambda l: not l.lstrip().startswith("#include"), lines))

    lines_from_first_include = [l for l in lines_from_first_include if not l.lstrip().startswith("#include")]  # remove existing includes

    for index, include_line in enumerate(include_lines):
        include_lines[index] = str(include_line.lstrip()).replace("<", '"').replace(">", '"')

    for index, include_line in enumerate(include_lines):
        include_lines[index] = include_line.split('"')[0] + '"' + include_line.split('"')[1] + '"\n'

    include_lines.sort(key=lambda line: line.upper())

    standard_libraries = [
        '"algorithm"',
        '"cassert"',
        '"chrono"',
        '"cmath"',
        '"cstring"',
        '"ctype.h"',
        '"filesystem"',
        '"functional"',
        '"inttypes.h"',
        '"iomanip"',
        '"iostream"',
        '"iostream.h"',
        '"limits"',
        '"list"',
        '"map"',
        '"memory"',
        '"mutex"',
        '"numeric"',
        '"optional"',
        '"Python.h"',
        '"ranges"',
        '"regex"',
        '"span"',
        '"sstream"',
        '"stdarg.h"',
        '"stdbool.h"',
        '"stdint.h"',
        '"stdio.h"',
        '"stdlib.h"',
        '"string"',
        '"string.h"',
        '"thread"',
        '"time.h"',
        '"type_traits"',
        '"unordered_map"',
        '"unordered_set"',
        '"variant"',
        '"vector"',
    ]

    standard_libraries.extend(
        [
            "BinaryData.h",
            "juce_gui_basics/juce_gui_basics.h",
            "juce_gui_extra/juce_gui_extra.h",
            "juce_opengl/juce_opengl",
        ]
    )

    for index, include_line in enumerate(include_lines):
        if any(s in include_line for s in standard_libraries):
            include_lines[index] = include_line.replace('"', ">").replace(" >", " <")

    with open(path, "w") as file:
        for line in lines_before_first_include:
            file.write(line)
        for line in include_lines:
            file.write(line)
        for line in lines_from_first_include:
            file.write(line)
