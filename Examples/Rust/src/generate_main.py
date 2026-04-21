import sys
from pathlib import Path

sys.path.append(str(Path("../../..")))  # location of helpers.py

import helpers

examples = sorted(p.stem for p in Path("Examples").rglob("*") if p.is_file() and p.suffix == ".rs")

examples = [s for s in examples if s != "mod"]

keys = [chr(ord("A") + i) for i in range(len(examples))]

mod_lines = "\n".join(f"pub mod {e};" for e in examples)

Path("examples/mod.rs").write_text(f"{helpers.preamble()}{mod_lines}\n")

examples = [s for s in examples if s != "connection"]

printlns = "\n".join(f'    println!("{k}. {e}.rs");' for k, e in zip(keys, examples))

match_arms = "\n".join(f"        '{k}' => {e}::run()," for k, e in zip(keys, examples))

Path("main.rs").write_text(f"""\
{helpers.preamble()}use crate::examples::*;

mod examples;
mod helpers;

fn main() {{
    println!("Select example");
{printlns}

    match helpers::get_key() {{
{match_arms}
        _ => {{}}
    }}
}}
""")
