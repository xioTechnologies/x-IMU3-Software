from pathlib import Path


def extract(line: str, start_token: str, end_token: str) -> str:
    return line.split(start_token)[1].split(end_token)[0]


lines = Path("ximu3/ximu3.c").read_text().splitlines()

constants = [extract(l, '"', '"') for l in lines if "PyModule_AddIntConstant" in l]

functions = [extract(l, ", ", ")") for l in lines if "PyModule_AddFunctions" in l]

objects = [extract(l, "&", ",") for l in lines if "add_object(module" in l]

Path("ximu3-stubs/__init__.pyi").write_text(f"""\
{"\n".join(f"{c}: int" for c in constants)}

{"\n\n".join(f"# {f}\n# TODO" for f in functions)}

{"\n\n".join(f"# {o}\nclass {o.replace('object', '').replace('_', ' ').title().replace(' ', '')}: ...  # TODO" for o in objects)}
""")
