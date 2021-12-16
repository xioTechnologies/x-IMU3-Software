import json
import os
import sys

sys.path.append(os.path.join("..", "..", "..", ".."))  # location of helpers.py

import helpers

json_file_path = "generate_data_messages.json"

# Generate *_message.rs
with open(json_file_path) as file:
    for json_object in json.load(file):

        if json_object["argumentNames"] != []:
            with open("template_float.txt") as text_file:
                template = text_file.read()

            arguments_struct = ""
            arguments_scan_format = ""
            arguments_types = ""
            arguments_list = ""
            arguments_packed_struct = ""
            arguments_assign_struct = ""
            arguments_csv_heading = ""
            arguments_csv_format = ""
            arguments_ascii_format = ""
            arguments_self_list = ""
            arguments_string_format = ""

            argument_names = json_object["argumentNames"]
            argument_units = json_object["argumentUnits"]

            for index in range(len(argument_names)):
                argument_name_snake_case = helpers.snake_case(argument_names[index])

                arguments_struct += "pub " + argument_name_snake_case + ": f32,\n    "
                arguments_scan_format += "{f},"
                arguments_types += "f32, "
                arguments_list += argument_name_snake_case + ", "
                arguments_packed_struct += argument_name_snake_case + ": f32,\n            "
                arguments_assign_struct += argument_name_snake_case + ": binary_message." + argument_name_snake_case + ", "
                arguments_csv_heading += argument_names[index] + ("," if argument_units[index] == "" else " (" + argument_units[index] + "),")
                arguments_csv_format += "{:.6},"
                arguments_ascii_format += "{:.4},"
                arguments_self_list += "self." + argument_name_snake_case + ", "
                arguments_string_format += " {:>8.3}" + ("" if argument_units[index] == "" else " " + argument_units[index])

            arguments_struct = arguments_struct[:-5]  # remove trailing new line and indentation
            arguments_scan_format = arguments_scan_format[:-1]  # remove trailing comma
            arguments_types = arguments_types[:-2]  # remove trailing comma and space
            arguments_list = arguments_list[:-2]  # remove trailing comma and space
            arguments_packed_struct = arguments_packed_struct[:-13]  # remove trailing new line and indentation
            arguments_assign_struct = arguments_assign_struct[:-2]  # remove trailing comma and space
            arguments_csv_heading = arguments_csv_heading[:-1]  # remove trailing comma
            arguments_csv_format = arguments_csv_format[:-1]  # remove trailing comma
            arguments_ascii_format = arguments_ascii_format[:-1]  # remove trailing comma
            arguments_self_list = arguments_self_list[:-2]  # remove trailing comma and space

            template = template.replace("$ArgumentsStruct$", arguments_struct)
            template = template.replace("$ArgumentsScanFormat$", arguments_scan_format)
            template = template.replace("$ArgumentsTypes$", arguments_types)
            template = template.replace("$ArgumentsList$", arguments_list)
            template = template.replace("$ArgumentsPackedStruct$", arguments_packed_struct)
            template = template.replace("$ArgumentsAssignStruct$", arguments_assign_struct)
            template = template.replace("$ArgumentsCsvFormat$", arguments_csv_format)
            template = template.replace("$CsvHeadings$", arguments_csv_heading)
            template = template.replace("$ArgumentsAsciiFormat$", arguments_ascii_format)
            template = template.replace("$ArgumentsSelfList$", arguments_self_list)
            template = template.replace("$ArgumentsStringFormat$", arguments_string_format)
        else:
            with open("template_char_array.txt") as text_file:
                template = text_file.read()

        template = template.replace("$NamePascalCase$", helpers.pascal_case(json_object["name"]))
        template = template.replace("$NameSnakeCase$", helpers.snake_case(json_object["name"]))
        template = template.replace("$AsciiID$", json_object["asciiID"])

        with open(helpers.snake_case(json_object["name"]) + "_message.rs", "w") as source_file:
            source_file.write(helpers.preamble())
            source_file.write(template)

# Generate mod.rs
with open("mod.rs", "w") as source_file:
    source_file.write(helpers.preamble())

    source_file.write("use self::helpers::*;\n")
    source_file.write("pub use self::data_message::*;\n")

    with open(json_file_path) as file:
        for json_object in json.load(file):
            source_file.write("pub use self::" + helpers.snake_case(json_object["name"]) + "_message::*;\n")

    source_file.write("\n")
    source_file.write("mod helpers;\n")
    source_file.write("pub mod data_message;\n")

    with open(json_file_path) as file:
        for json_object in json.load(file):
            source_file.write("pub mod " + helpers.snake_case(json_object["name"]) + "_message;\n")


def insert(file_path, template, id):
    with open(json_file_path) as file:
        code = ""

        for json_object in json.load(file):
            modifed = template

            modifed = modifed.replace("$NameCamelCase$", helpers.camel_case(json_object["name"]))
            modifed = modifed.replace("$NamePascalCase$", helpers.pascal_case(json_object["name"]))
            modifed = modifed.replace("$NameSnakeCase$", helpers.snake_case(json_object["name"]))

            code += modifed

    helpers.insert(file_path, code, id)


# Insert code into x-IMU3-API/Rust/src/connection.rs
template = "\
\n\
    pub fn add_$NameSnakeCase$_closure(&mut self, closure: Box<dyn Fn($NamePascalCase$Message) + Send>) -> u64 {\n\
        self.internal.lock().unwrap().get_decoder().lock().unwrap().dispatcher.add_$NameSnakeCase$_closure(closure)\n\
    }\n"

insert("../connection.rs", template, "0")

# Insert code into x-IMU3-API/Rust/src/ffi/connection.rs
template = "\
\n\
#[no_mangle]\n\
pub extern \"C\" fn XIMU3_connection_add_$NameSnakeCase$_callback(connection: *mut Connection, callback: Callback<$NamePascalCase$Message>, context: *mut c_void) -> u64 {\n\
    let connection: &mut Connection = unsafe { &mut *connection };\n\
    let void_ptr = VoidPtr(context);\n\
    connection.add_$NameSnakeCase$_closure(Box::new(move |message: $NamePascalCase$Message| callback(message, void_ptr.0)))\n\
}\n"

insert("../ffi/connection.rs", template, "0")

# Insert code into x-IMU3-API/Rust/src/decoder.rs
template = "        parse!($NamePascalCase$Message, $NamePascalCase$);\n"

insert("../decoder.rs", template, "0")

# Insert code into x-IMU3-API/Rust/src/dispatcher.rs
file_path = "../dispatcher.rs"

template = "    $NamePascalCase$($NamePascalCase$Message),\n"

insert(file_path, template, "0")

template = "    $NameSnakeCase$_closures: Arc<Mutex<Vec<(Box<dyn Fn($NamePascalCase$Message) + Send>, u64)>>>,\n"

insert(file_path, template, "1")

template = "            $NameSnakeCase$_closures: Arc::new(Mutex::new(Vec::new())),\n"

insert(file_path, template, "2")

template = "        let $NameSnakeCase$_closures = dispatcher.$NameSnakeCase$_closures.clone();\n"

insert(file_path, template, "3")

template = "\
                        DispatcherData::$NamePascalCase$(message) => {\n\
                            data_closures.lock().unwrap().iter().for_each(|(closure, _)| closure(Box::new(message)));\n\
                            $NameSnakeCase$_closures.lock().unwrap().iter().for_each(|(closure, _)| closure(message));\n\
                        }\n"

insert(file_path, template, "4")

template = "\
\n\
    pub fn add_$NameSnakeCase$_closure(&mut self, closure: Box<dyn Fn($NamePascalCase$Message) + Send>) -> u64 {\n\
        let id = self.get_closure_id();\n\
        self.$NameSnakeCase$_closures.lock().unwrap().push((closure, id));\n\
        id\n\
    }\n"

insert(file_path, template, "5")

template = "        self.$NameSnakeCase$_closures.lock().unwrap().retain(|(_, id)| id != &closure_id);\n"

insert(file_path, template, "6")

# Insert code into x-IMU3-API/Rust/src/ffi/data_messages.rs
template = "\
\n\
#[no_mangle]\n\
pub extern \"C\" fn XIMU3_$NameSnakeCase$_message_to_string(message: $NamePascalCase$Message) -> *const c_char {\n\
    string_to_char_ptr!(message.to_string())\n\
}\n"

insert("../ffi/data_messages.rs", template, "0")

# Insert code into x-IMU3-API/Cpp/Connection.hpp
template = "\n\
        uint64_t add$NamePascalCase$Callback(std::function<void(XIMU3_$NamePascalCase$Message)>& callback)\n\
        {\n\
            return XIMU3_connection_add_$NameSnakeCase$_callback(connection, Helpers::wrapCallable<XIMU3_$NamePascalCase$Message>(callback), &callback);\n\
        }\n"

insert("../../../Cpp/Connection.hpp", template, "0")

# Generate x-IMU3-API/Python/Python-C-API/DataMessages/*Message.h
directory = "../../../Python/Python-C-API/DataMessages/"

with open(json_file_path) as file:
    for json_object in json.load(file):
        width = len("timestamp")

        for name in json_object["argumentNames"]:
            width = max(width, len(name))

        if json_object["argumentNames"] != []:
            with open(directory + "TemplateFloat.txt") as text_file:
                template = text_file.read()

            get_functions = ""
            get_set_members = ""

            for name in json_object["argumentNames"]:
                get_function = "\
static PyObject* $NameSnakeCase$_message_get_$Argument$($NamePascalCase$Message* self)\n\
{\n\
    return Py_BuildValue(\"f\", self->message.$Argument$);\n\
}\n\n"
                get_functions += get_function.replace("$Argument$", helpers.snake_case(name))

                get_set_member = "{ \"$Argument$\", $WhiteSpace$(getter) $NameSnakeCase$_message_get_$Argument$, $WhiteSpace$NULL, \"\", NULL },\n        "
                get_set_member = get_set_member.replace("$Argument$", helpers.snake_case(name))
                get_set_member = get_set_member.replace("$WhiteSpace$", "".ljust(width - len(helpers.snake_case(name))))

                get_set_members += get_set_member

            get_functions = get_functions[:-2]  # remove trailing new line
            get_set_members = get_set_members[:-9]  # remove trailing new line and indentation

            template = template.replace("$GetFunctions$", get_functions)
            template = template.replace("$GetSetMembers$", get_set_members)
            template = template.replace("$WhiteSpace$", "".ljust(width - len("timestamp")))
        else:
            with open(directory + "TemplateCharArray.txt") as text_file:
                template = text_file.read()

        template = template.replace("$NameMacroCase$", helpers.macro_case(json_object["name"]))
        template = template.replace("$NamePascalCase$", helpers.pascal_case(json_object["name"]))
        template = template.replace("$NameSnakeCase$", helpers.snake_case(json_object["name"]))

        with open(directory + helpers.pascal_case(json_object["name"]) + "Message.h", "w") as source_file:
            source_file.write(helpers.preamble())
            source_file.write(template)

# Generate x-IMU3-API/Python/Python-C-API/DataMessages/DataMessages.h
with open("../../../Python/Python-C-API/DataMessages/DataMessages.h", "w") as header_file:
    header_file.write(helpers.preamble())

    with open(json_file_path) as file:
        for json_object in json.load(file):
            header_file.write("#include \"" + helpers.pascal_case(json_object["name"]) + "Message.h\"\n")

# Insert code into x-IMU3-API/Python/Python-C-API/ximu3.c
template = "        add_type(module, &$NameSnakeCase$_message_type, \"$NamePascalCase$Message\") &&\n"

insert("../../../Python/Python-C-API/ximu3.c", template, "0")

# Insert code into x-IMU3-API/Python/Python-C-API/Connection.h
file_path = "../../../Python/Python-C-API/Connection.h"

template = "\n\
static PyObject* connection_add_$NameSnakeCase$_callback(Connection* self, PyObject* args)\n\
{\n\
    PyObject* callable;\n\
\n\
    if (PyArg_ParseTuple(args, \"O:set_callback\", &callable) == 0)\n\
    {\n\
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);\n\
        return NULL;\n\
    }\n\
\n\
    if (PyCallable_Check(callable) == 0)\n\
    {\n\
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);\n\
        return NULL;\n\
    }\n\
\n\
    Py_INCREF(callable); // this will never be destroyed (memory leak)\n\
\n\
    return Py_BuildValue(\"K\", XIMU3_connection_add_$NameSnakeCase$_callback(self->connection, $NameSnakeCase$_message_callback, callable));\n\
}\n"

insert(file_path, template, "0")

with open(json_file_path) as file:
    code = ""

    for json_object in json.load(file):
        template = "        { \"add_$NameSnakeCase$_callback\", $WhiteSpace$(PyCFunction) connection_add_$NameSnakeCase$_callback, $WhiteSpace$METH_VARARGS, \"\" },\n"

        template = template.replace("$NameSnakeCase$", helpers.snake_case(json_object["name"]))
        template = template.replace("$WhiteSpace$", "".ljust(19 - len(json_object["name"])))

        code += template

    helpers.insert(file_path, code, "1")

# Generate x-IMU3-API/CSharp/x-IMU3/DataMessages/*Message.h
directory = "../../../CSharp/x-IMU3/DataMessages/"

with open(json_file_path) as file:
    for json_object in json.load(file):
        if json_object["argumentNames"] != []:
            with open(directory + "TemplateFloat.txt") as text_file:
                template = text_file.read()

            get_methods = ""

            for name in json_object["argumentNames"]:
                get_function = "\
        property float $ArgumentPascalCase$\n\
        {\n\
            float get()\n\
            {\n\
                return message->$ArgumentSnakeCase$;\n\
            }\n\
        }\n\n"
                get_methods += get_function.replace("$ArgumentPascalCase$", helpers.pascal_case(name)).replace("$ArgumentSnakeCase$", helpers.snake_case(name))

            get_methods = get_methods[:-2]  # remove trailing new line

            template = template.replace("$GetMethods$", get_methods)
        else:
            with open(directory + "TemplateCharArray.txt") as text_file:
                template = text_file.read()

        template = template.replace("$NamePascalCase$", helpers.pascal_case(json_object["name"]))
        template = template.replace("$NameSnakeCase$", helpers.snake_case(json_object["name"]))

        with open(directory + helpers.pascal_case(json_object["name"]) + "Message.h", "w") as source_file:
            source_file.write(helpers.preamble())
            source_file.write(template)

# Generate x-IMU3-API/CSharp/x-IMU3/DataMessages/DataMessages.h
with open("../../../CSharp/x-IMU3/DataMessages/DataMessages.h", "w") as header_file:
    header_file.write(helpers.preamble())

    with open(json_file_path) as file:
        for json_object in json.load(file):
            header_file.write("#include \"" + helpers.pascal_case(json_object["name"]) + "Message.h\"\n")

# Insert code into x-IMU3-API/CSharp/x-IMU3/EventArgs.h
template = "\n\
    public ref class $NamePascalCase$EventArgs : public EventArgs\n\
    {\n\
    public:\n\
        $NamePascalCase$Message^ message;\n\
\n\
    internal:\n\
        $NamePascalCase$EventArgs($NamePascalCase$Message^ message) : message(message)\n\
        {\n\
        }\n\
    };\n"

insert("../../../CSharp/x-IMU3/EventArgs.h", template, "0")

# Insert code into x-IMU3-API/CSharp/x-IMU3/Connection.h
file_path = "../../../CSharp/x-IMU3/Connection.h"

template = "        event EventHandler<$NamePascalCase$EventArgs^>^ $NamePascalCase$Event;\n"

insert(file_path, template, "0")

template = "            ximu3::XIMU3_connection_add_$NameSnakeCase$_callback(connection, static_cast<ximu3::XIMU3_Callback$NamePascalCase$Message>(Marshal::GetFunctionPointerForDelegate($NameCamelCase$Delegate).ToPointer()), GCHandle::ToIntPtr(thisHandle).ToPointer());\n"

insert(file_path, template, "1")

template = "\n\
        delegate void $NamePascalCase$Delegate(ximu3::XIMU3_$NamePascalCase$Message data, void* context);\n\
\n\
        static void $NamePascalCase$Callback(ximu3::XIMU3_$NamePascalCase$Message data, void* context)\n\
        {\n\
            auto sender = GCHandle::FromIntPtr(IntPtr(context)).Target;\n\
            static_cast<Connection^>(sender)->$NamePascalCase$Event(sender, gcnew $NamePascalCase$EventArgs(gcnew $NamePascalCase$Message(data)));\n\
        }\n\
\n\
        const $NamePascalCase$Delegate^ $NameCamelCase$Delegate = gcnew $NamePascalCase$Delegate($NamePascalCase$Callback);\n"

insert(file_path, template, "2")
