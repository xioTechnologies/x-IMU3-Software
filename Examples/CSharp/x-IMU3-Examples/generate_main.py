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

examples = [s for s in examples if s != "Connection"]

keys = [chr(ord("A") + i) for i in range(len(examples))]

with open("Program.cs", "w") as file:
    file.write(helpers.preamble())

    file.write("namespace Ximu3Examples\n")
    file.write("{\n")
    file.write("    class Program\n")
    file.write("    {\n")
    file.write("        static int Main()\n")
    file.write("        {\n")
    file.write('            Console.WriteLine("Select example");\n')

    for key, example in zip(keys, examples):
        file.write(f'            Console.WriteLine("{key}. {example}");\n')

    file.write("            switch (Helpers.GetKey())\n")
    file.write("            {\n")

    for key, example in zip(keys, examples):
        file.write(f"                case '{key}':\n")
        file.write(f"                    _ = new {example}();\n")
        file.write("                    break;\n")

    file.write("            }\n")
    file.write('            Console.WriteLine("Press any key to exit");\n')
    file.write("            Helpers.GetKey();\n")
    file.write("            return 0;\n")
    file.write("        }\n")
    file.write("    }\n")
    file.write("}\n")
