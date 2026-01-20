def get_blocks(c_code: str, declaration: str) -> list[str]:
    blocks = []
    block = ""
    in_block = False

    for line in c_code.splitlines():
        if (not in_block) and (declaration in line) and (";" not in line):
            in_block = True

        if not in_block:
            continue

        block += line + "\n"

        if "}" in line:
            blocks.append(block)
            block = ""
            in_block = False

    return blocks


# Read C file
with open("../../C/Ximu3.h") as file:
    c_code = file.read()

cs_code = ""

# Enums
for block in get_blocks(c_code, "typedef enum"):
    block = block.replace("typedef", "public")
    block = block[: block.index("}")] + "}\n\n"

    cs_code += block

# Structs
for block in get_blocks(c_code, "typedef struct"):
    block = block.replace("typedef", "public")
    block = block[: block.index("}")] + "}\n\n"

    block = block.replace("size_t", "public UIntPtr")
    block = block.replace("uint16_t", "public UInt16")
    block = block.replace("uint32_t", "public UInt32")
    block = block.replace("uint64_t", "public UInt64")
    block = block.replace("int32_t", "public Int32")
    block = block.replace("float", "public float")
    block = block.replace("bool", "public bool")
    block = block.replace("enum", "public")
    block = block.replace("    struct", "    public")

    block = "\n".join([p if "*array" not in p else "    public IntPtr array;" for p in block.splitlines()])

    block = block.replace("char char_array", "[MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE)]\n    public byte[] char_array")
    block = block.replace("[XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE]", "")

    block = block.replace("char ", "[MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]\n    public byte[] ")
    block = block.replace("[XIMU3_CHAR_ARRAY_SIZE]", "")

    block = block.replace("interface", "interface_")

    cs_code += "[StructLayout(LayoutKind.Sequential)]\n"
    cs_code += block + "\n"

# Callbacks
for line in c_code.splitlines():
    if "typedef void (*" not in line:
        continue

    line = line.replace("typedef void (*", "public delegate void ")
    line = line.replace(")(", "(")
    line = line.replace("struct ", "")
    line = line.replace("enum ", "")
    line = line.replace("void *", "IntPtr ")

    cs_code += "[UnmanagedFunctionPointer(CallingConvention.Cdecl)]\n"
    cs_code += line + "\n\n"

cs_code += """\
[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
public delegate void XIMU3_CallbackEndOfFile(IntPtr context);\n
"""

# Functions
for line in c_code.splitlines():
    if not line.endswith(");"):
        continue

    if line.startswith("typedef"):
        continue

    line = line.replace("uint8_t", "Byte")
    line = line.replace("uint32_t", "UInt32")
    line = line.replace("uint64_t", "UInt64")
    line = line.replace("enum ", "")
    line = line.replace("struct ", "")
    line = line.replace("(void);", "();")

    line = line.replace("void (*callback)(void *context)", "XIMU3_CallbackEndOfFile callback")

    for pattern in [
        "const char *const *",
        "const char *",
        "const XIMU3_MuxConnectionConfig *",
        "void *",
        "XIMU3_Connection *const *",
        "XIMU3_Connection *",
        "XIMU3_DataLogger *",
        "XIMU3_FileConverter *",
        "XIMU3_KeepOpen *",
        "XIMU3_MuxConnectionConfig *",
        "XIMU3_NetworkAnnouncement *",
        "XIMU3_PortScanner *",
    ]:
        line = line.replace(pattern, "IntPtr ")

    cs_code += '[DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]\n'
    cs_code += "public static extern " + line + "\n\n"

# Write C# file
cs_code = "\n".join([f"        {p}" for p in cs_code.splitlines() if p])

cs_code = f"""\
using System;
using System.Runtime.InteropServices;

namespace Ximu3
{{
    public static class CApi
    {{
        public const int XIMU3_DEFAULT_RETRIES = 2;
        public const int XIMU3_DEFAULT_TIMEOUT = 500;
        public const int XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE = 256;
        public const int XIMU3_CHAR_ARRAY_SIZE = 256;

{cs_code}
    }}
}}"""

with open("CApi.cs", "w") as file:
    file.write(cs_code)
