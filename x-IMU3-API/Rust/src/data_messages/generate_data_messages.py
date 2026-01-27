import os
import sys
from dataclasses import dataclass

sys.path.append(os.path.join("..", "..", "..", ".."))  # location of helpers.py

import helpers


@dataclass(frozen=True)
class Message:
    name: str
    ascii_id: str
    argument_names: list[str]
    argument_units: list[str]


messages = [
    Message(
        name="Inertial",
        ascii_id="I",
        argument_names=["Gyroscope X", "Gyroscope Y", "Gyroscope Z", "Accelerometer X", "Accelerometer Y", "Accelerometer Z"],
        argument_units=["deg/s", "deg/s", "deg/s", "g", "g", "g"],
    ),
    Message(
        name="Magnetometer",
        ascii_id="M",
        argument_names=["X", "Y", "Z"],
        argument_units=["a.u.", "a.u.", "a.u."],
    ),
    Message(
        name="Quaternion",
        ascii_id="Q",
        argument_names=["W", "X", "Y", "Z"],
        argument_units=["", "", "", ""],
    ),
    Message(
        name="Rotation Matrix",
        ascii_id="R",
        argument_names=["XX", "XY", "XZ", "YX", "YY", "YZ", "ZX", "ZY", "ZZ"],
        argument_units=["", "", "", "", "", "", "", "", ""],
    ),
    Message(
        name="Euler Angles",
        ascii_id="A",
        argument_names=["Roll", "Pitch", "Yaw"],
        argument_units=["deg", "deg", "deg"],
    ),
    Message(
        name="Linear Acceleration",
        ascii_id="L",
        argument_names=["Quaternion W", "Quaternion X", "Quaternion Y", "Quaternion Z", "Acceleration X", "Acceleration Y", "Acceleration Z"],
        argument_units=["", "", "", "", "g", "g", "g"],
    ),
    Message(
        name="Earth Acceleration",
        ascii_id="E",
        argument_names=["Quaternion W", "Quaternion X", "Quaternion Y", "Quaternion Z", "Acceleration X", "Acceleration Y", "Acceleration Z"],
        argument_units=["", "", "", "", "g", "g", "g"],
    ),
    Message(
        name="AHRS status",
        ascii_id="U",
        argument_names=["Initialising", "Angular Rate Recovery", "Acceleration Recovery", "Magnetic Recovery"],
        argument_units=["", "", "", ""],
    ),
    Message(
        name="High-g accelerometer",
        ascii_id="H",
        argument_names=["X", "Y", "Z"],
        argument_units=["g", "g", "g"],
    ),
    Message(
        name="Temperature",
        ascii_id="T",
        argument_names=["Temperature"],
        argument_units=["degC"],
    ),
    Message(
        name="Battery",
        ascii_id="B",
        argument_names=["Percentage", "Voltage", "Charging Status"],
        argument_units=["%", "V", ""],
    ),
    Message(
        name="RSSI",
        ascii_id="W",
        argument_names=["Percentage", "Power"],
        argument_units=["%", "dBm"],
    ),
    Message(
        name="Serial accessory",
        ascii_id="S",
        argument_names=[],
        argument_units=[],
    ),
    Message(
        name="Notification",
        ascii_id="N",
        argument_names=[],
        argument_units=[],
    ),
    Message(
        name="Error",
        ascii_id="F",
        argument_names=[],
        argument_units=[],
    ),
]

# Generate *_message.rs
for message in messages:
    if message.argument_names:
        with open("template_float.txt") as file:
            template = file.read()

        arguments_struct_declare = "".join(["pub " + helpers.snake_case(n) + ": f32,\n    " for n in message.argument_names]).rstrip("\n    ")
        arguments_scan_fmt = "".join(["{f}," for _ in message.argument_names]).rstrip(",")
        arguments_types = "".join(["f32, " for _ in message.argument_names]).rstrip(", ")
        arguments_scan_result = "".join([helpers.snake_case(n) + ", " for n in message.argument_names]).rstrip(", ")
        arguments_struct_asign = "".join([helpers.snake_case(n) + ",\n                " for n in message.argument_names]).rstrip("\n                ")
        arguments_packed_struct = "".join([helpers.snake_case(n) + ": f32,\n            " for n in message.argument_names]).rstrip("\n            ")
        arguments_assign_struct = "".join([helpers.snake_case(n) + ": binary_message." + helpers.snake_case(n) + ",\n            " for n in message.argument_names]).rstrip("\n            ")
        arguments_csv_heading = "".join([n + ("," if u == "" else " (" + u + "),") for n, u in zip(message.argument_names, message.argument_units)]).rstrip(",")
        arguments_csv_format = "".join(["{:.6}," for _ in message.argument_names]).rstrip(",")
        arguments_ascii_format = "".join(["{:.4}," for _ in message.argument_names]).rstrip(",")
        arguments_self_list = "".join(["self." + helpers.snake_case(n) + ", " for n in message.argument_names]).rstrip(", ")
        arguments_string_format = "".join([" {:>8.3}" + ("" if u == "" else " " + u) for u in message.argument_units]).rstrip(", ")

        template = template.replace("$arguments_struct_declare$", arguments_struct_declare)
        template = template.replace("$arguments_scan_fmt$", arguments_scan_fmt)
        template = template.replace("$arguments_types$", arguments_types)
        template = template.replace("$arguments_scan_result$", arguments_scan_result)
        template = template.replace("$arguments_struct_asign$", arguments_struct_asign)
        template = template.replace("$arguments_packed_struct$", arguments_packed_struct)
        template = template.replace("$arguments_assign_struct$", arguments_assign_struct)
        template = template.replace("$arguments_csv_format$", arguments_csv_format)
        template = template.replace("$csv_headings$", arguments_csv_heading)
        template = template.replace("$arguments_ascii_format$", arguments_ascii_format)
        template = template.replace("$arguments_self_list$", arguments_self_list)
        template = template.replace("$arguments_string_format$", arguments_string_format)
    else:
        with open("template_char_array.txt") as file:
            template = file.read()

    template = template.replace("$name_pascal_case$", helpers.pascal_case(message.name))
    template = template.replace("$name_snake_case$", helpers.snake_case(message.name))
    template = template.replace("$ascii_id$", message.ascii_id)

    with open(helpers.snake_case(message.name) + "_message.rs", "w") as file:
        file.write(helpers.preamble())
        file.write(template)


def insert(file_path: str, template: str, id: int) -> None:
    code = ""

    for message in messages:
        modified = template

        modified = modified.replace("$name_camel_case$", helpers.camel_case(message.name))
        modified = modified.replace("$name_pascal_case$", helpers.pascal_case(message.name))
        modified = modified.replace("$name_snake_case$", helpers.snake_case(message.name))

        code += modified

    helpers.insert(file_path, code, id)


# Insert code into mod.rs
file_path = "mod.rs"

template = "pub use self::$name_snake_case$_message::*;\n"

insert(file_path, template, 0)

template = "pub mod $name_snake_case$_message;\n"

insert(file_path, template, 1)

# Insert code into x-IMU3-API/Rust/src/connection.rs
template = """
    pub fn add_$name_snake_case$_closure(&self, closure: Box<dyn Fn($name_pascal_case$Message) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_$name_snake_case$_closure(closure)
    }\n"""

insert("../connection.rs", template, 0)

# Insert code into x-IMU3-API/Rust/src/ffi/connection.rs
template = """
#[no_mangle]
pub extern "C" fn XIMU3_connection_add_$name_snake_case$_callback(connection: *mut Connection, callback: Callback<$name_pascal_case$Message>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_$name_snake_case$_closure(Box::new(move |message| callback(message, void_ptr.0)))
}\n"""

insert("../ffi/connection.rs", template, 0)

# Insert code into x-IMU3-API/Rust/src/receiver.rs
template = "        parse!($name_pascal_case$Message, $name_pascal_case$);\n"

insert("../receiver.rs", template, 0)

# Insert code into x-IMU3-API/Rust/src/dispatcher.rs
file_path = "../dispatcher.rs"

template = "    $name_pascal_case$($name_pascal_case$Message),\n"

insert(file_path, template, 0)

template = "    $name_snake_case$_closures: Arc<Mutex<Vec<(Box<dyn Fn($name_pascal_case$Message) + Send>, u64)>>>,\n"

insert(file_path, template, 1)

template = "            $name_snake_case$_closures: Arc::new(Mutex::new(Vec::new())),\n"

insert(file_path, template, 2)

template = "        let $name_snake_case$_closures = dispatcher.$name_snake_case$_closures.clone();\n"

insert(file_path, template, 3)

template = """\
                DispatcherData::$name_pascal_case$(message) => {
                    data_closures.lock().unwrap().iter().for_each(|(closure, _)| closure(Box::new(message)));
                    $name_snake_case$_closures.lock().unwrap().iter().for_each(|(closure, _)| closure(message));
                }\n"""

insert(file_path, template, 4)

template = """
    pub fn add_$name_snake_case$_closure(&self, closure: Box<dyn Fn($name_pascal_case$Message) + Send>) -> u64 {
        let id = self.get_closure_id();
        self.$name_snake_case$_closures.lock().unwrap().push((closure, id));
        id
    }\n"""

insert(file_path, template, 5)

template = "        self.$name_snake_case$_closures.lock().unwrap().retain(|(_, id)| id != &closure_id);\n"

insert(file_path, template, 6)

# Insert code into x-IMU3-API/Rust/src/ffi/data_messages.rs
template = """
#[no_mangle]
pub extern "C" fn XIMU3_$name_snake_case$_message_to_string(message: $name_pascal_case$Message) -> *const c_char {
    str_to_char_ptr(&message.to_string())
}\n"""

insert("../ffi/data_messages.rs", template, 0)

# Insert code into x-IMU3-API/Cpp/Connection.hpp
template = """\
        uint64_t add$name_pascal_case$Callback(std::function<void(XIMU3_$name_pascal_case$Message)> &callback) {
            return XIMU3_connection_add_$name_snake_case$_callback(wrapped, Helpers::wrapCallable<XIMU3_$name_pascal_case$Message>(callback), &callback);
        }\n\n"""

insert("../../../Cpp/Connection.hpp", template, 0)

# Generate x-IMU3-API/Python/ximu3/DataMessages/*Message.h
directory = "../../../Python/ximu3/DataMessages/"

for message in messages:
    if message.argument_names:
        with open(directory + "TemplateFloat.txt") as file:
            template = file.read()

        # Get functions
        argument_names = ["Timestamp"] + message.argument_names

        get_functions = ""

        for argument_type, argument_name in zip(["PyLong_FromUnsignedLongLong((unsigned long long) "] + ["PyFloat_FromDouble((double) " for _ in message.argument_names], argument_names):
            get_function = """\
static PyObject *$name_snake_case$_message_get_$argument_name$($name_pascal_case$Message *self) {
    return $argument_type$self->wrapped.$argument_name$);
}\n\n"""
            get_function = get_function.replace("$argument_type$", argument_type)
            get_function = get_function.replace("$argument_name$", helpers.snake_case(argument_name))
            get_functions += get_function

        template = template.replace("$get_functions$", get_functions.rstrip("\n"))

        # Method functions
        method_names = []

        method_functions = ""

        if message.name in ["Quaternion", "Rotation Matrix", "Linear Acceleration", "Earth Acceleration"]:
            method_names = method_names + ["to_euler_angles_message"]
            method_functions += "static PyObject *$name_snake_case$_message_to_euler_angles_message($name_pascal_case$Message *self, PyObject *args);\n\n"

        if message.name == "Euler Angles":
            method_names = method_names + ["to_quaternion_message"]
            method_functions += "static PyObject *$name_snake_case$_message_to_quaternion_message($name_pascal_case$Message *self, PyObject *args);\n\n"

        template = template.replace("$method_functions$", method_functions.rstrip("\n"))

        # Get set members
        get_set_members = ""

        for argument_name in argument_names:
            get_set_member = '{"$argument_name$", (getter) $name_snake_case$_message_get_$argument_name$, NULL, "", NULL},\n    '
            get_set_member = get_set_member.replace("$argument_name$", helpers.snake_case(argument_name))
            get_set_members += get_set_member

        template = template.replace("$get_set_members$", get_set_members.rstrip("\n    "))

        # Get method members
        method_members = ""

        for method_name in method_names:
            method_member = '{"$method_name$", (PyCFunction) $name_snake_case$_message_$method_name$, METH_NOARGS, ""},\n    '
            method_member = method_member.replace("$method_name$", method_name)
            method_members += method_member

        template = template.replace("$method_members$", method_members.rstrip("\n    "))
    else:
        with open(directory + "TemplateCharArray.txt") as file:
            template = file.read()

    template = template.replace("$name_macro_case$", helpers.macro_case(message.name))
    template = template.replace("$name_pascal_case$", helpers.pascal_case(message.name))
    template = template.replace("$name_snake_case$", helpers.snake_case(message.name))

    with open(directory + helpers.pascal_case(message.name) + "Message.h", "w") as file:
        file.write(helpers.preamble())
        file.write(template)

# Generate x-IMU3-API/Python/ximu3/DataMessages/DataMessages.h
insert("../../../Python/ximu3/DataMessages/DataMessages.h", '#include "$name_pascal_case$Message.h"\n', 0)

# Insert code into x-IMU3-API/Python/ximu3/ximu3.c
template = '        add_object(module, &$name_snake_case$_message_object, "$name_pascal_case$Message") &&\n'

insert("../../../Python/ximu3/ximu3.c", template, 0)

# Insert code into x-IMU3-API/Python/ximu3/Connection.h
file_path = "../../../Python/ximu3/Connection.h"

template = """\
static PyObject *connection_add_$name_snake_case$_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_$name_snake_case$_callback(self->wrapped, $name_snake_case$_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}\n\n"""

insert(file_path, template, 0)

code = ""

for message in messages:
    template = '    {"add_$name_snake_case$_callback", (PyCFunction) connection_add_$name_snake_case$_callback, METH_O, ""},\n'

    template = template.replace("$name_snake_case$", helpers.snake_case(message.name))

    code += template

helpers.insert(file_path, code, 1)

# Insert code into x-IMU3-API/CSharp/x-IMU3/Connection.h
file_path = "../../../CSharp/x-IMU3/Connection.cs"

template = """\
        public delegate void $name_pascal_case$Callback(CApi.XIMU3_$name_pascal_case$Message message);

        private static void $name_pascal_case$CallbackInternal(CApi.XIMU3_$name_pascal_case$Message message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<$name_pascal_case$Callback>(context)(message);
        }

        public UInt64 Add$name_pascal_case$Callback($name_pascal_case$Callback callback)
        {
            return CApi.XIMU3_connection_add_$name_snake_case$_callback(wrapped, $name_pascal_case$CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }\n\n"""

insert(file_path, template, 0)
