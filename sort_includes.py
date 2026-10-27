from itertools import dropwhile, takewhile
from pathlib import Path

paths = [path for path in Path(__file__).resolve().parent.rglob("*") if path.suffix in (".h", ".hpp", ".c", ".cpp")]

paths = [path for path in paths if not any("cmake-build-" in part for part in path.parts)]

for path in paths:
    lines = path.read_text().splitlines(keepends=True)

    lines = [l.rstrip() + "\n" for l in lines]

    include_lines = [l for l in lines if l.lstrip().startswith("#include")]

    lines_before_first_include = list(takewhile(lambda l: not l.lstrip().startswith("#include"), lines))

    lines_from_first_include = list(dropwhile(lambda l: not l.lstrip().startswith("#include"), lines))

    lines_from_first_include = [l for l in lines_from_first_include if not l.lstrip().startswith("#include")]  # remove existing includes

    for index, include_line in enumerate(include_lines):
        include_lines[index] = str(include_line.lstrip()).replace("<", '"').replace(">", '"')

    include_lines.sort(key=lambda line: ("Python.h" not in line, line.upper()))  # Python.h must be first

    standard_libraries = [
        '"algorithm"',
        '"atomic"',
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
        '"utility"',
        '"variant"',
        '"vector"',
    ]

    standard_libraries.extend(
        [
            "BinaryData.h",
            "juce_core/juce_core.h",
            "juce_data_structures/juce_data_structures.h",
            "juce_gui_basics/juce_gui_basics.h",
            "juce_gui_extra/juce_gui_extra.h",
            "juce_opengl/juce_opengl",
            "Python.h",
        ]
    )

    for index, include_line in enumerate(include_lines):
        if any(s in include_line for s in standard_libraries):
            include_lines[index] = include_line.replace('"', ">").replace(" >", " <")

    path.write_text("".join(lines_before_first_include + include_lines + lines_from_first_include))
