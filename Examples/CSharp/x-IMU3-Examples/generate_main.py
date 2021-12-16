import os
import sys

sys.path.append(os.path.join("..", "..", ".."))  # location of helpers.py

import helpers

examples = []

for _, _, file_names in os.walk("Examples"):
    for file_name in file_names:
        file_name_without_extension, extension = os.path.splitext(file_name)

        if extension != ".cs" or file_name_without_extension == "Connection":
            continue

        examples.append(file_name_without_extension)

examples = sorted(examples)

with open("Program.cs", "w") as file:
    file.write(helpers.preamble())

    file.write("using System;\n")
    file.write("\n")
    file.write("namespace Ximu3Examples\n")
    file.write("{\n")
    file.write("    class Program\n")
    file.write("    {\n")
    file.write("        static int Main(string[] args)\n")
    file.write("        {\n")
    file.write("            Console.WriteLine(\"Select example\");\n")

    key = "A"
    for example in examples:
        file.write("            Console.WriteLine(\"" + key + ". " + example + "\");\n")
        key = chr(ord(key) + 1)

    file.write("            switch (Helpers.GetKey())\n")
    file.write("            {\n")

    key = "A"
    for example in examples:
        file.write("                case '" + key + "':\n")
        file.write("                    new " + example + "();\n")
        file.write("                    break;\n")
        key = chr(ord(key) + 1)

    file.write("            }\n")
    file.write("            Console.WriteLine(\"Press any key to exit\");\n")
    file.write("            Helpers.GetKey();\n")
    file.write("            return 0;\n")
    file.write("        }\n")
    file.write("    }\n")
    file.write("}\n")
