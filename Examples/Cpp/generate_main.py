import os
import sys

sys.path.append(os.path.join("..", ".."))  # location of helpers.py

import helpers

examples = []

for _, _, file_names in os.walk("Examples"):
    for file_name in file_names:
        file_name_without_extension, extension = os.path.splitext(file_name)

        if extension != ".h" or file_name_without_extension == "Connection":
            continue

        examples.append(file_name_without_extension)

examples = sorted(examples)

with open("main.cpp", "w") as file:
    file.write(helpers.preamble())

    for declaration in examples:
        file.write("#include \"Examples/" + declaration + ".h\"\n")

    file.write("#include \"Helpers.hpp\"\n")
    file.write("#include <iostream>\n\n")

    file.write("int main(int argc, const char* argv[])\n{\n")
    file.write("    setbuf(stdout, NULL);\n")
    file.write("    std::cout << \"Select example \" << std::endl;\n")

    key = "A"
    for declaration in examples:
        file.write("    std::cout << \"" + key + ". " + declaration + ".h\" << std::endl;\n")
        key = chr(ord(key) + 1)

    file.write("    switch (helpers::getKey())\n")
    file.write("    {\n")

    key = "A"
    for declaration in examples:
        file.write("        case \'" + str(key) + "\':\n")
        file.write("            " + declaration + "();\n")
        file.write("            break;\n")
        key = chr(ord(key) + 1)

    file.write("    }\n")
    file.write("}\n")
