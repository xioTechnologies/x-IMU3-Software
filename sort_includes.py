import os

for root, _, files in os.walk(os.path.dirname(os.path.realpath(__file__))):
    for file in files:
        file_name, file_extension = os.path.splitext(file)

        if "cmake-build-" in root:
            continue

        if file_extension != ".h" and file_extension != ".hpp" and file_extension != ".c" and file_extension != ".cpp":
            continue

        # Read file
        file_path = os.path.join(root, file)
        with open(file_path) as file:
            try:
                all_lines = file.readlines()
            except:
                continue

        # Extract lines
        include_lines = []
        lines_before = []
        lines_after = []

        waiting_for_includes = True

        for line in all_lines:
            line = line.rstrip() + "\n"  # remove trailing witespace
            if line.lstrip().startswith("#include"):
                include_lines.append(str(line.lstrip()).replace("<", "\"").replace(">", "\""))
                waiting_for_includes = False
                continue
            if waiting_for_includes:
                lines_before.append(line)
            else:
                lines_after.append(line)

        # Alphabetise includes
        include_lines.sort(key=lambda line: line.upper())

        # Use angle brackets for standard libraries
        standard_libraries = ["\"algorithm\"", "\"cassert\"", "\"chrono\"", "\"cmath\"", "\"cstring\"", "\"ctype.h\"", "\"filesystem\"", "\"functional\"", "\"inttypes.h\"",
                              "\"iostream\"", "\"iostream.h\"", "\"limits\"", "\"list\"", "\"map\"", "\"memory\"", "\"mutex\"", "\"numeric\"", "\"optional\"", "\"Python.h\"",
                              "\"regex\"", "\"stdarg.h\"", "\"stdbool.h\"", "\"stdint.h\"", "\"stdio.h\"", "\"stdlib.h\"", "\"string\"", "\"string.h\"", "\"thread\"",
                              "\"time.h\"", "\"type_traits\"", "\"unordered_map\"", "\"variant\"", "\"vector\""]

        standard_libraries.extend(["BinaryData.h", "juce_gui_basics/juce_gui_basics.h", "juce_gui_extra/juce_gui_extra.h", "juce_opengl/juce_opengl"])

        for index, _ in enumerate(include_lines):
            for standard_library in standard_libraries:
                if standard_library in include_lines[index]:
                    include_lines[index] = include_lines[index].replace("\"", ">").replace(" >", " <")

        # Overwrite original file
        with open(file_path, "w") as file:
            for line in lines_before:
                file.write(line)
            for line in include_lines:
                file.write(line)
            for line in lines_after:
                file.write(line)
