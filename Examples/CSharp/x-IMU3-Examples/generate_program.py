import sys
from pathlib import Path

sys.path.append(str(Path("../../..")))  # location of helpers.py

import helpers

examples = sorted(p.stem for p in Path("Examples").rglob("*") if p.is_file() and p.suffix == ".cs")

examples = [s for s in examples if s != "Connection"]

keys = [chr(ord("A") + i) for i in range(len(examples))]

writelines = "\n".join(f'            Console.WriteLine("{k}. {e}");' for k, e in zip(keys, examples))

cases = "\n".join(
    f"""\
                case '{k}':
                    _ = new {e}();
                    break;"""
    for k, e in zip(keys, examples)
)

Path("Program.cs").write_text(f"""\
{helpers.preamble()}namespace Ximu3Examples
{{
    class Program
    {{
        static int Main()
        {{
            Console.WriteLine("Select example");
{writelines}

            switch (Helpers.GetKey())
            {{
{cases}
            }}

            return 0;
        }}
    }}
}}
""")
