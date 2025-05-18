import os
import sys

sys.path.append(os.path.join("..", ".."))  # location of helpers.py

import helpers

examples = []

for _, _, file_names in os.walk("Examples"):
    for file_name in file_names:
        file_stem, extension = os.path.splitext(file_name)

        examples.append(file_stem)

examples = sorted(examples)

examples = [s for s in examples if s != "Connection"]

keys = [chr(ord("A") + i) for i in range(len(examples))]

with open("main.cpp", "w") as file:
    file.write(helpers.preamble())

    for example in examples:
        file.write(f'#include "Examples/{example}.h"\n')

    file.write('#include "Helpers.hpp"\n')
    file.write("#include <iostream>\n\n")

    file.write("int main(int argc, const char* argv[])\n{\n")
    file.write("    setbuf(stdout, NULL);\n\n")
    file.write('    std::cout << "Select example " << std::endl;\n')

    for key, example in zip(keys, examples):
        file.write(f'    std::cout << "{key}. {example}.h" << std::endl;\n')

    file.write("\n")
    file.write("    switch (helpers::getKey())\n")
    file.write("    {\n")

    for key, example in zip(keys, examples):
        file.write(f"        case '{key}':\n")
        file.write(f"            {example}();\n")
        file.write("            break;\n")

    file.write("    }\n")
    file.write("}\n")
