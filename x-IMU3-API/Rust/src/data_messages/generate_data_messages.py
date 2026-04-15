import os
import sys
from dataclasses import dataclass

sys.path.append(os.path.join("..", "..", "..", ".."))  # location of helpers.py

import helpers


@dataclass(frozen=True)
class Message:
    name: str
    ascii_id: str
    argument_names: tuple[str]
    argument_units: tuple[str]


messages = [
    Message(
        name="Inertial",
        ascii_id="I",
        argument_names=("Gyroscope X", "Gyroscope Y", "Gyroscope Z", "Accelerometer X", "Accelerometer Y", "Accelerometer Z"),
        argument_units=("deg/s", "deg/s", "deg/s", "g", "g", "g"),
    ),
    Message(
        name="Magnetometer",
        ascii_id="M",
        argument_names=("X", "Y", "Z"),
        argument_units=("a.u.", "a.u.", "a.u."),
    ),
    Message(
        name="High-g accelerometer",
        ascii_id="H",
        argument_names=("X", "Y", "Z"),
        argument_units=("g", "g", "g"),
    ),
    Message(
        name="Quaternion",
        ascii_id="Q",
        argument_names=("W", "X", "Y", "Z"),
        argument_units=("", "", "", ""),
    ),
    Message(
        name="Rotation Matrix",
        ascii_id="R",
        argument_names=("XX", "XY", "XZ", "YX", "YY", "YZ", "ZX", "ZY", "ZZ"),
        argument_units=("", "", "", "", "", "", "", "", ""),
    ),
    Message(
        name="Euler Angles",
        ascii_id="A",
        argument_names=("Roll", "Pitch", "Yaw"),
        argument_units=("deg", "deg", "deg"),
    ),
    Message(
        name="Linear Acceleration",
        ascii_id="L",
        argument_names=("Quaternion W", "Quaternion X", "Quaternion Y", "Quaternion Z", "Acceleration X", "Acceleration Y", "Acceleration Z"),
        argument_units=("", "", "", "", "g", "g", "g"),
    ),
    Message(
        name="Earth Acceleration",
        ascii_id="E",
        argument_names=("Quaternion W", "Quaternion X", "Quaternion Y", "Quaternion Z", "Acceleration X", "Acceleration Y", "Acceleration Z"),
        argument_units=("", "", "", "", "g", "g", "g"),
    ),
    Message(
        name="AHRS status",
        ascii_id="U",
        argument_names=("Initialising", "Angular Rate Recovery", "Acceleration Recovery", "Magnetic Recovery"),
        argument_units=("", "", "", ""),
    ),
    Message(
        name="Serial accessory",
        ascii_id="S",
        argument_names=(),
        argument_units=(),
    ),
    Message(
        name="Sync",
        ascii_id="Y",
        argument_names=("Edge",),
        argument_units=("",),
    ),
    Message(
        name="LTC",
        ascii_id="C",
        argument_names=(),
        argument_units=(),
    ),
    Message(
        name="Temperature",
        ascii_id="T",
        argument_names=("Temperature",),
        argument_units=("degC",),
    ),
    Message(
        name="Battery",
        ascii_id="B",
        argument_names=("Percentage", "Voltage", "Charging Status"),
        argument_units=("%", "V", ""),
    ),
    Message(
        name="RSSI",
        ascii_id="W",
        argument_names=("Percentage", "Power"),
        argument_units=("%", "dBm"),
    ),
    Message(
        name="Button",
        ascii_id="O",
        argument_names=("State",),
        argument_units=("",),
    ),
    Message(
        name="Notification",
        ascii_id="N",
        argument_names=(),
        argument_units=(),
    ),
    Message(
        name="Error",
        ascii_id="F",
        argument_names=(),
        argument_units=(),
    ),
]

# Generate *_message.rs
for message in messages:
    path = "template_float.txt" if message.argument_names else "template_char_array.txt"

    with open(path) as file:
        code = file.read()

    code = helpers.replace(
        code,
        (
            ("$name_snake_case$", helpers.snake_case(message.name)),
            ("$name_pascal_case$", helpers.pascal_case(message.name)),
            ("$name_camel_case$", helpers.camel_case(message.name)),
            ("$ascii_id$", message.ascii_id),
            ("$arguments_struct_declare$", "".join("pub " + helpers.snake_case(n) + ": f32,\n    " for n in message.argument_names).rstrip()),
            ("$arguments_struct_default$", "".join([helpers.snake_case(n) + ": 0.0,\n            " for n in message.argument_names]).rstrip("\n    ")),
            ("$arguments_scan_fmt$", "".join("{f}," for _ in message.argument_names).rstrip(",")),
            ("$arguments_types$", "".join("f32, " for _ in message.argument_names).rstrip(", ")),
            ("$arguments_scan_result$", "".join(helpers.snake_case(n) + ", " for n in message.argument_names).rstrip(", ")),
            ("$arguments_struct_asign$", "".join(helpers.snake_case(n) + ",\n                " for n in message.argument_names).rstrip()),
            ("$arguments_packed_struct$", "".join(helpers.snake_case(n) + ": f32,\n            " for n in message.argument_names).rstrip()),
            ("$arguments_assign_struct$", "".join(helpers.snake_case(n) + ": binary_message." + helpers.snake_case(n) + ",\n            " for n in message.argument_names).rstrip()),
            ("$arguments_csv_format$", "".join("{:.6}," for _ in message.argument_names).rstrip(",")),
            ("$csv_headings$", "".join(n + ("," if u == "" else " (" + u + "),") for n, u in zip(message.argument_names, message.argument_units)).rstrip(",")),
            ("$arguments_ascii_format$", "".join("{:.4}," for _ in message.argument_names).rstrip(",")),
            ("$arguments_self_list$", "".join("self." + helpers.snake_case(n) + ", " for n in message.argument_names).rstrip(", ")),
            ("$arguments_string_format$", "".join(" {:>8.3}" + ("" if u == "" else " " + u) for u in message.argument_units).rstrip(", ")),
        ),
    )

    with open(helpers.snake_case(message.name) + "_message.rs", "w") as file:
        file.write(helpers.preamble())
        file.write(code)


def insert(path: str, id: int, template: str) -> None:
    code = "".join(
        helpers.replace(
            template,
            (
                ("$name_snake_case$", helpers.snake_case(m.name)),
                ("$name_pascal_case$", helpers.pascal_case(m.name)),
                ("$name_camel_case$", helpers.camel_case(m.name)),
            ),
        )
        for m in messages
    )

    helpers.insert(path, id, code)


# Insert code into mod.rs
path = "mod.rs"

insert(
    path,
    0,
    "pub use self::$name_snake_case$_message::*;\n",
)

insert(
    path,
    1,
    "pub mod $name_snake_case$_message;\n",
)

# Insert code into x-IMU3-API/Rust/src/connection.rs
insert(
    "../connection.rs",
    0,
    """\
    pub fn get_$name_snake_case$_message(&self, consume: bool) -> Option<$name_pascal_case$Message> {
        if consume {
            return self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.$name_snake_case$_message.lock().unwrap().take();
        }
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.$name_snake_case$_message.lock().unwrap().clone()
    }\n\n""",
)

insert(
    "../connection.rs",
    1,
    """\
    pub fn add_$name_snake_case$_closure(&self, closure: Box<dyn Fn($name_pascal_case$Message) + Send>) -> u64 {
        self.internal.lock().unwrap().get_receiver().lock().unwrap().dispatcher.add_$name_snake_case$_closure(closure)
    }\n\n""",
)

# Insert code into x-IMU3-API/Rust/src/ffi/connection.rs
insert(
    "../ffi/connection.rs",
    0,
    """\
#[no_mangle]
pub extern "C" fn XIMU3_connection_get_$name_snake_case$_message(connection: *mut Connection, consume: bool) -> $name_pascal_case$Message {
    let connection = unsafe { &*connection };
    connection.get_$name_snake_case$_message(consume).unwrap_or_default()
}\n\n""",
)

insert(
    "../ffi/connection.rs",
    1,
    """\
#[no_mangle]
pub extern "C" fn XIMU3_connection_add_$name_snake_case$_callback(connection: *mut Connection, callback: Callback<$name_pascal_case$Message>, context: *mut c_void) -> u64 {
    let connection = unsafe { &*connection };
    let void_ptr = VoidPtr(context);
    connection.add_$name_snake_case$_closure(Box::new(move |message| callback(message, void_ptr.0)))
}\n\n""",
)

# Insert code into x-IMU3-API/Rust/src/receiver.rs
insert(
    "../receiver.rs",
    0,
    "        parse!($name_pascal_case$Message, $name_pascal_case$);\n",
)

# Insert code into x-IMU3-API/Rust/src/dispatcher.rs
path = "../dispatcher.rs"

insert(
    path,
    0,
    "    $name_pascal_case$($name_pascal_case$Message),\n",
)

insert(
    path,
    1,
    "    pub $name_snake_case$_message: Arc<Mutex<Option<$name_pascal_case$Message>>>,\n",
)

insert(
    path,
    2,
    "    $name_snake_case$_closures: Arc<Mutex<Vec<(Box<dyn Fn($name_pascal_case$Message) + Send>, u64)>>>,\n",
)

insert(
    path,
    3,
    "            $name_snake_case$_message: Arc::new(Mutex::new(None)),\n",
)

insert(
    path,
    4,
    "            $name_snake_case$_closures: Arc::new(Mutex::new(Vec::new())),\n",
)

insert(
    path,
    5,
    "        let $name_snake_case$_message = dispatcher.$name_snake_case$_message.clone();\n",
)

insert(
    path,
    6,
    "        let $name_snake_case$_closures = dispatcher.$name_snake_case$_closures.clone();\n",
)

insert(
    path,
    7,
    """\
                DispatcherData::$name_pascal_case$(message) => {
                    *$name_snake_case$_message.lock().unwrap() = Some(message);                
                    data_closures.lock().unwrap().iter().for_each(|(closure, _)| closure(Box::new(message)));
                    $name_snake_case$_closures.lock().unwrap().iter().for_each(|(closure, _)| closure(message));
                }\n""",
)

insert(
    path,
    8,
    """\
    pub fn add_$name_snake_case$_closure(&self, closure: Box<dyn Fn($name_pascal_case$Message) + Send>) -> u64 {
        let id = self.get_closure_id();
        self.$name_snake_case$_closures.lock().unwrap().push((closure, id));
        id
    }\n\n""",
)

insert(
    path,
    9,
    "        self.$name_snake_case$_closures.lock().unwrap().retain(|(_, id)| id != &closure_id);\n",
)

# Insert code into x-IMU3-API/Rust/src/ffi/data_messages.rs
insert(
    "../ffi/data_messages.rs",
    0,
    """\
#[no_mangle]
pub extern "C" fn XIMU3_$name_snake_case$_message_to_string(message: $name_pascal_case$Message) -> *const c_char {
    str_to_char_ptr(&message.to_string())
}\n\n""",
)

# Insert code into x-IMU3-API/Cpp/Connection.hpp
insert(
    "../../../Cpp/Connection.hpp",
    0,
    """\
        XIMU3_$name_pascal_case$Message get$name_pascal_case$Message(bool consume = false) {
            return XIMU3_connection_get_$name_snake_case$_message(wrapped, consume);
        }\n\n""",
)

insert(
    "../../../Cpp/Connection.hpp",
    1,
    """\
        uint64_t add$name_pascal_case$Callback(std::function<void(XIMU3_$name_pascal_case$Message)> &callback) {
            return XIMU3_connection_add_$name_snake_case$_callback(wrapped, Helpers::wrapCallable<XIMU3_$name_pascal_case$Message>(callback), &callback);
        }\n\n""",
)


# Generate x-IMU3-API/Python/ximu3/DataMessages/*Message.h
directory = "../../../Python/ximu3/DataMessages/"

for message in messages:
    if message.name in ("Quaternion", "Rotation Matrix", "Euler Angles", "Linear Acceleration", "Earth Acceleration"):
        file_name = "TemplateFloatMethods.txt"
    elif message.argument_names:
        file_name = "TemplateFloat.txt"
    else:
        file_name = "TemplateCharArray.txt"

    path = os.path.join(directory, file_name)

    with open(path) as file:
        code = file.read()

    argument_names = ("Timestamp", *message.argument_names)
    argument_types = ("PyLong_FromUnsignedLongLong((unsigned long long) ",) + tuple("PyFloat_FromDouble((double) " for _ in message.argument_names)

    get_functions = "".join(
        helpers.replace(
            """\
static PyObject *$name_snake_case$_message_get_$argument_name$($name_pascal_case$Message *self) {
    return $argument_type$self->wrapped.$argument_name$);
}\n\n""",
            (
                ("$argument_type$", argument_type),
                ("$argument_name$", helpers.snake_case(argument_name)),
            ),
        )
        for argument_name, argument_type in zip(argument_names, argument_types)
    ).rstrip()

    get_set_members = "".join(
        helpers.replace(
            '{"$argument_name$", (getter) $name_snake_case$_message_get_$argument_name$, NULL, "", NULL},\n    ',
            (
                ("$argument_type$", argument_type),
                ("$argument_name$", helpers.snake_case(argument_name)),
            ),
        )
        for argument_name, argument_type in zip(argument_names, argument_types)
    ).rstrip()

    if message.name == "Euler Angles":
        method_functions = "static PyObject *$name_snake_case$_message_to_quaternion_message($name_pascal_case$Message *self, PyObject *args);"
        method_members = '{"to_quaternion_message", (PyCFunction) $name_snake_case$_message_to_quaternion_message, METH_NOARGS, ""},'
    else:
        method_functions = "static PyObject *$name_snake_case$_message_to_euler_angles_message($name_pascal_case$Message *self, PyObject *args);"
        method_members = '{"to_euler_angles_message", (PyCFunction) $name_snake_case$_message_to_euler_angles_message, METH_NOARGS, ""},'

    code = helpers.replace(
        code,
        (
            ("$get_functions$", get_functions),
            ("$method_functions$", method_functions),
            ("$get_set_members$", get_set_members),
            ("$method_members$", method_members),
            ("$name_snake_case$", helpers.snake_case(message.name)),
            ("$name_macro_case$", helpers.macro_case(message.name)),
            ("$name_pascal_case$", helpers.pascal_case(message.name)),
        ),
    )

    with open(os.path.join(directory, f"{helpers.pascal_case(message.name)}Message.h"), "w") as file:
        file.write(helpers.preamble())
        file.write(code)

# Generate x-IMU3-API/Python/ximu3/DataMessages/DataMessages.h
insert(
    "../../../Python/ximu3/DataMessages/DataMessages.h",
    0,
    '#include "$name_pascal_case$Message.h"\n',
)

# Insert code into x-IMU3-API/Python/ximu3/ximu3.c
insert(
    "../../../Python/ximu3/ximu3.c",
    0,
    '        add_object(module, &$name_snake_case$_message_object, "$name_pascal_case$Message") &&\n',
)

# Insert code into x-IMU3-API/Python/ximu3/Connection.h
path = "../../../Python/ximu3/Connection.h"

insert(
    path,
    0,
    """\
static PyObject *connection_get_$name_snake_case$_message(Connection *self, PyObject *args, PyObject *kwds) {
    int consume = false;

    static char *kwlist[] = {
        "consume",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "|p", kwlist, &consume) == 0) {
        return NULL;
    }

    const XIMU3_$name_pascal_case$Message message = XIMU3_connection_get_$name_snake_case$_message(self->wrapped, (bool) consume);

    if (message.timestamp == 0) {
        Py_RETURN_NONE;
    }

    return $name_snake_case$_message_from(&message);
}\n\n""",
)

insert(
    path,
    1,
    """\
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
}\n\n""",
)

insert(
    path,
    2,
    '    {"get_$name_snake_case$_message", (PyCFunction) connection_get_$name_snake_case$_message, METH_VARARGS | METH_KEYWORDS, ""},\n',
)

insert(
    path,
    3,
    '    {"add_$name_snake_case$_callback", (PyCFunction) connection_add_$name_snake_case$_callback, METH_O, ""},\n',
)

# Insert code into x-IMU3-API/CSharp/x-IMU3/Connection.h
insert(
    "../../../CSharp/x-IMU3/Connection.cs",
    0,
    """\
        public CApi.XIMU3_$name_pascal_case$Message Get$name_pascal_case$Message(bool consume=false)
        {
            return CApi.XIMU3_connection_get_$name_snake_case$_message(wrapped, consume);
        }\n\n""",
)

insert(
    "../../../CSharp/x-IMU3/Connection.cs",
    1,
    """\
        public delegate void $name_pascal_case$Callback(CApi.XIMU3_$name_pascal_case$Message message);

        private static void $name_pascal_case$CallbackInternal(CApi.XIMU3_$name_pascal_case$Message message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<$name_pascal_case$Callback>(context)(message);
        }

        public UInt64 Add$name_pascal_case$Callback($name_pascal_case$Callback callback)
        {
            return CApi.XIMU3_connection_add_$name_snake_case$_callback(wrapped, $name_pascal_case$CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }\n\n""",
)
