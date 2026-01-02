import os
import sys

sys.path.append(os.path.join("..", ".."))  # location of helpers.py

import helpers

examples = []

for _, _, file_names in os.walk("examples"):
    for file_name in file_names:
        file_stem, extension = os.path.splitext(file_name)

        if extension == ".c":
            examples.append(file_stem)

examples = sorted(examples)

examples = [s for s in examples if s != "connection"]

keys = [chr(ord("A") + i) for i in range(len(examples))]

with open("main.c", "w") as file:
    file.write(helpers.preamble())

    file.write('#include "helpers.h"\n')
    file.write("#include <stdio.h>\n\n")

    for example in examples:
        file.write(f"void {example}();\n\n")

    file.write("int main(int argc, const char* argv[])\n{\n")
    file.write('    printf("Select example\\n");\n')

    for key, example in zip(keys, examples):
        file.write(f'    printf("{key}. {example}.c\\n");\n')

    file.write("\n")
    file.write("    switch (get_key())\n")
    file.write("    {\n")

    for key, example in zip(keys, examples):
        file.write(f"        case '{key}':\n")
        file.write(f"            {example}();\n")
        file.write("            break;\n")

    file.write("    }\n")
    file.write("}\n")
