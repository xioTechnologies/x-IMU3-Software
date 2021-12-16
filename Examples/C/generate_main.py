import os
import sys

sys.path.append(os.path.join("..", ".."))  # location of helpers.py

import helpers

examples = []

for _, _, file_names in os.walk("Examples"):
    for file_name in file_names:
        file_name_without_extension, extension = os.path.splitext(file_name)

        if extension != ".c" or file_name_without_extension == "Connection":
            continue

        examples.append(file_name_without_extension)

examples = sorted(examples)

with open("main.c", "w") as file:
    file.write(helpers.preamble())

    file.write("#include \"Helpers.h\"\n")
    file.write("#include <stdio.h>\n\n")

    for declaration in examples:
        file.write("void " + declaration + "();\n\n")

    file.write("int main(int argc, const char* argv[])\n{\n")
    file.write("    printf(\"Select example\\n\");\n")

    key = "A"
    for declaration in examples:
        file.write("    printf(\"" + key + ". " + declaration + ".c\\n\");\n")
        key = chr(ord(key) + 1)

    file.write("    switch (GetKey())\n")
    file.write("    {\n")

    key = "A"
    for declaration in examples:
        file.write("        case \'" + str(key) + "\':\n")
        file.write("            " + declaration + "();\n")
        file.write("            break;\n")
        key = chr(ord(key) + 1)

    file.write("    }\n")
    file.write("}\n")
