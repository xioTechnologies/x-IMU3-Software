import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

examples = []

for _, _, file_names in os.walk("examples"):
    for file_name in file_names:
        file_name_without_extension = os.path.splitext(file_name)[0]

        if file_name_without_extension == "connection":
            continue

        examples.append(file_name_without_extension)

examples = sorted(examples)
examples.remove("mod")

with open("examples/mod.rs", "w") as file:
    file.write(helpers.preamble())

    file.write("mod connection;\n")

    for example in examples:
        file.write("pub mod " + example + ";\n")

with open("main.rs", "w") as file:
    file.write(helpers.preamble())

    file.write("use crate::examples::*;\n\n")
    file.write("mod examples;\n")
    file.write("mod helpers;\n\n")
    file.write("fn main() {\n")
    file.write("    println!(\"Select example\");\n")

    key = "A"
    for example in examples:
        file.write("    println!(\"" + key + ". " + example + ".rs\");\n")
        key = chr(ord(key) + 1)

    file.write("\n")
    file.write("    match helpers::get_key() {\n")

    key = "A"
    for example in examples:
        file.write("        \'" + str(key) + "\' => " + example + "::run(),\n")
        key = chr(ord(key) + 1)

    file.write("        _ => {}\n")
    file.write("    }\n")
    file.write("}\n")
