import sys
from pathlib import Path

sys.path.append(str(Path("../..")))  # location of helpers.py

import helpers

examples = sorted(p.stem for p in Path("examples").rglob("*") if p.is_file() and p.suffix == ".c")

examples = [s for s in examples if s != "connection"]

keys = [chr(ord("A") + i) for i in range(len(examples))]

declarations = "\n\n".join(f"void {e}();" for e in examples)

printfs = "\n".join(f'    printf("{k}. {e}.c\\n");' for k, e in zip(keys, examples))

cases = "\n".join(
    f"""\
        case '{key}':
            {example}();
            break;"""
    for key, example in zip(keys, examples)
)

Path("main.c").write_text(f"""\
{helpers.preamble()}#include "helpers.h"
#include <stdio.h>

{declarations}

int main(int argc, const char *argv[]) {{
    printf("Select example\\n");
{printfs}

    switch (get_key()) {{
{cases}
    }}
}}
""")
