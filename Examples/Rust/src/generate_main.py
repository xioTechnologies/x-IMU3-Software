import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

examples = []

for _, _, file_names in os.walk("Examples"):
    for file_name in file_names:
        file_stem, extension = os.path.splitext(file_name)

        examples.append(file_stem)

examples = sorted(examples)

examples = [s for s in examples if s not in ["mod"]]

keys = [chr(ord("A") + i) for i in range(len(examples))]

with open("examples/mod.rs", "w") as file:
    file.write(helpers.preamble())

    for example in examples:
        file.write(f"pub mod {example};\n")

examples = [s for s in examples if s not in ["connection"]]

with open("main.rs", "w") as file:
    file.write(helpers.preamble())

    file.write("use crate::examples::*;\n\n")
    file.write("mod examples;\n")
    file.write("mod helpers;\n\n")
    file.write("fn main() {\n")
    file.write('    println!("Select example");\n')

    for key, example in zip(keys, examples):
        file.write(f'    println!("{key}. {example}.rs");\n')

    file.write("\n")
    file.write("    match helpers::get_key() {\n")

    for key, example in zip(keys, examples):
        file.write(f"        '{key}' => {example}::run(),\n")

    file.write("        _ => {}\n")
    file.write("    }\n")
    file.write("}\n")
