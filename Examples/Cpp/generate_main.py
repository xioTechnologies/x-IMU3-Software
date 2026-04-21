import sys
from pathlib import Path

sys.path.append(str(Path("../..")))  # location of helpers.py

import helpers

examples = sorted(p.stem for p in Path("Examples").rglob("*") if p.is_file() and p.suffix == ".h")

examples = [s for s in examples if s != "Connection"]

keys = [chr(ord("A") + i) for i in range(len(examples))]

includes = "\n".join(f'#include "Examples/{e}.h"' for e in examples)

cout_examples = "\n".join(f'    std::cout << "{k}. {e}.h" << std::endl;' for k, e in zip(keys, examples))

cases = "\n".join(
    f"""\
        case '{k}':
            {e}();
            break;"""
    for k, e in zip(keys, examples)
)

Path("main.cpp").write_text(f"""\
{helpers.preamble()}{includes}
#include "Helpers.hpp"
#include <iostream>

int main(int argc, const char *argv[]) {{
    setbuf(stdout, NULL);

    std::cout << "Select example " << std::endl;
{cout_examples}

    switch (helpers::getKey()) {{
{cases}
    }}
}}
""")
