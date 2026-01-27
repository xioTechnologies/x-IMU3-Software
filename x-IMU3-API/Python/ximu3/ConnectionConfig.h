#ifndef CONNECTION_CONFIG_H
#define CONNECTION_CONFIG_H

#include "../../C/Ximu3.h"
#include <Python.h>

//----------------------------------------------------------------------------------------------------------------------
// USB connection config

typedef struct {
    PyObject_HEAD
    XIMU3_UsbConnectionConfig wrapped;
} UsbConnectionConfig;

static PyObject *usb_connection_config_str(UsbConnectionConfig *self) {
    const char *const string = XIMU3_usb_connection_config_to_string(self->wrapped);

    return PyUnicode_FromString(string);
}

static PyObject *usb_connection_config_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *port_name;

    static char *kwlist[] = {
        "port_name",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &port_name) == 0) {
        return NULL;
    }

    UsbConnectionConfig *const self = (UsbConnectionConfig *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->wrapped.port_name, sizeof(self->wrapped.port_name), "%s", port_name);

    return (PyObject *) self;
}

static void usb_connection_config_free(UsbConnectionConfig *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *usb_connection_config_get_port_name(UsbConnectionConfig *self, PyObject *args) {
    return PyUnicode_FromString(self->wrapped.port_name);
}

static int usb_connection_config_set_port_name(UsbConnectionConfig *self, PyObject *value, void *closure) {
    const char *const port_name = PyUnicode_AsUTF8(value);

    if (port_name == NULL) {
        return -1;
    }

    snprintf(self->wrapped.port_name, sizeof(self->wrapped.port_name), "%s", port_name);

    return 0;
}

static PyGetSetDef usb_connection_config_get_set[] = {
    {"port_name", (getter) usb_connection_config_get_port_name, (setter) usb_connection_config_set_port_name, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject usb_connection_config_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.UsbConnectionConfig",
    .tp_basicsize = sizeof(UsbConnectionConfig),
    .tp_dealloc = (destructor) usb_connection_config_free,
    .tp_str = (reprfunc) usb_connection_config_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = usb_connection_config_new,
    .tp_getset = usb_connection_config_get_set,
};

static PyObject *usb_connection_config_from(const XIMU3_UsbConnectionConfig *const config) {
    UsbConnectionConfig *const self = (UsbConnectionConfig *) usb_connection_config_object.tp_alloc(&usb_connection_config_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = *config;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// Serial connection config

typedef struct {
    PyObject_HEAD
    XIMU3_SerialConnectionConfig wrapped;
} SerialConnectionConfig;

static PyObject *serial_connection_config_str(SerialConnectionConfig *self) {
    const char *const string = XIMU3_serial_connection_config_to_string(self->wrapped);

    return PyUnicode_FromString(string);
}

static PyObject *serial_connection_config_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *port_name;
    unsigned long baud_rate;
    int rts_cts_enabled;

    static char *kwlist[] = {
        "port_name",
        "baud_rate",
        "rts_cts_enabled",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "skp", kwlist, &port_name, &baud_rate, &rts_cts_enabled) == 0) {
        return NULL;
    }

    SerialConnectionConfig *const self = (SerialConnectionConfig *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->wrapped.port_name, sizeof(self->wrapped.port_name), "%s", port_name);
    self->wrapped.baud_rate = (uint32_t) baud_rate;
    self->wrapped.rts_cts_enabled = (bool) rts_cts_enabled;

    return (PyObject *) self;
}

static void serial_connection_config_free(SerialConnectionConfig *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *serial_connection_config_get_port_name(SerialConnectionConfig *self, PyObject *args) {
    return PyUnicode_FromString(self->wrapped.port_name);
}

static int serial_connection_config_set_port_name(SerialConnectionConfig *self, PyObject *value, void *closure) {
    const char *const port_name = PyUnicode_AsUTF8(value);

    if (port_name == NULL) {
        return -1;
    }

    snprintf(self->wrapped.port_name, sizeof(self->wrapped.port_name), "%s", port_name);

    return 0;
}

static PyObject *serial_connection_config_get_baud_rate(SerialConnectionConfig *self, PyObject *args) {
    return PyLong_FromUnsignedLong((unsigned long) self->wrapped.baud_rate);
}

static int serial_connection_config_set_baud_rate(SerialConnectionConfig *self, PyObject *value, void *closure) {
    const uint32_t baud_rate = (uint32_t) PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->wrapped.baud_rate = baud_rate;

    return 0;
}

static PyObject *serial_connection_config_get_rts_cts_enabled(SerialConnectionConfig *self, PyObject *args) {
    return PyBool_FromLong((long) self->wrapped.rts_cts_enabled);
}

static int serial_connection_config_set_rts_cts_enabled(SerialConnectionConfig *self, PyObject *value, void *closure) {
    const bool rts_cts_enabled = PyObject_IsTrue(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->wrapped.rts_cts_enabled = rts_cts_enabled;

    return 0;
}

static PyGetSetDef serial_connection_config_get_set[] = {
    {"port_name", (getter) serial_connection_config_get_port_name, (setter) serial_connection_config_set_port_name, "", NULL},
    {"baud_rate", (getter) serial_connection_config_get_baud_rate, (setter) serial_connection_config_set_baud_rate, "", NULL},
    {"rts_cts_enabled", (getter) serial_connection_config_get_rts_cts_enabled, (setter) serial_connection_config_set_rts_cts_enabled, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject serial_connection_config_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.SerialConnectionConfig",
    .tp_basicsize = sizeof(SerialConnectionConfig),
    .tp_dealloc = (destructor) serial_connection_config_free,
    .tp_str = (reprfunc) serial_connection_config_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = serial_connection_config_new,
    .tp_getset = serial_connection_config_get_set,
};

static PyObject *serial_connection_config_from(const XIMU3_SerialConnectionConfig *const config) {
    SerialConnectionConfig *const self = (SerialConnectionConfig *) serial_connection_config_object.tp_alloc(&serial_connection_config_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = *config;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// TCP connection config

typedef struct {
    PyObject_HEAD
    XIMU3_TcpConnectionConfig wrapped;
} TcpConnectionConfig;

static PyObject *tcp_connection_config_str(TcpConnectionConfig *self) {
    const char *const string = XIMU3_tcp_connection_config_to_string(self->wrapped);

    return PyUnicode_FromString(string);
}

static PyObject *tcp_connection_config_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *ip_address;
    unsigned int port;

    static char *kwlist[] = {
        "ip_address",
        "port",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "sI", kwlist, &ip_address, &port) == 0) {
        return NULL;
    }

    TcpConnectionConfig *const self = (TcpConnectionConfig *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->wrapped.ip_address, sizeof(self->wrapped.ip_address), "%s", ip_address);
    self->wrapped.port = (uint16_t) port;

    return (PyObject *) self;
}

static void tcp_connection_config_free(TcpConnectionConfig *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *tcp_connection_config_get_ip_address(TcpConnectionConfig *self, PyObject *args) {
    return PyUnicode_FromString(self->wrapped.ip_address);
}

static int tcp_connection_config_set_ip_address(TcpConnectionConfig *self, PyObject *value, void *closure) {
    const char *const ip_address = PyUnicode_AsUTF8(value);

    if (ip_address == NULL) {
        return -1;
    }

    snprintf(self->wrapped.ip_address, sizeof(self->wrapped.ip_address), "%s", ip_address);

    return 0;
}

static PyObject *tcp_connection_config_get_port(TcpConnectionConfig *self, PyObject *args) {
    return PyLong_FromUnsignedLong((unsigned long) self->wrapped.port);
}

static int tcp_connection_config_set_port(TcpConnectionConfig *self, PyObject *value, void *closure) {
    const uint16_t port = (uint16_t) PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->wrapped.port = port;

    return 0;
}

static PyGetSetDef tcp_connection_config_get_set[] = {
    {"ip_address", (getter) tcp_connection_config_get_ip_address, (setter) tcp_connection_config_set_ip_address, "", NULL},
    {"port", (getter) tcp_connection_config_get_port, (setter) tcp_connection_config_set_port, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject tcp_connection_config_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.TcpConnectionConfig",
    .tp_basicsize = sizeof(TcpConnectionConfig),
    .tp_dealloc = (destructor) tcp_connection_config_free,
    .tp_str = (reprfunc) tcp_connection_config_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = tcp_connection_config_new,
    .tp_getset = tcp_connection_config_get_set,
};

static PyObject *tcp_connection_config_from(const XIMU3_TcpConnectionConfig *const config) {
    TcpConnectionConfig *const self = (TcpConnectionConfig *) tcp_connection_config_object.tp_alloc(&tcp_connection_config_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = *config;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// UDP connection config

typedef struct {
    PyObject_HEAD
    XIMU3_UdpConnectionConfig wrapped;
} UdpConnectionConfig;

static PyObject *udp_connection_config_str(UdpConnectionConfig *self) {
    const char *const string = XIMU3_udp_connection_config_to_string(self->wrapped);

    return PyUnicode_FromString(string);
}

static PyObject *udp_connection_config_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *ip_address;
    unsigned int send_port;
    unsigned int receive_port;

    static char *kwlist[] = {
        "ip_address",
        "send_port",
        "receive_port",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "sII", kwlist, &ip_address, &send_port, &receive_port) == 0) {
        return NULL;
    }

    UdpConnectionConfig *const self = (UdpConnectionConfig *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->wrapped.ip_address, sizeof(self->wrapped.ip_address), "%s", ip_address);
    self->wrapped.send_port = (uint16_t) send_port;
    self->wrapped.receive_port = (uint16_t) receive_port;

    return (PyObject *) self;
}

static void udp_connection_config_free(UdpConnectionConfig *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *udp_connection_config_get_ip_address(UdpConnectionConfig *self, PyObject *args) {
    return PyUnicode_FromString(self->wrapped.ip_address);
}

static int udp_connection_config_set_ip_address(UdpConnectionConfig *self, PyObject *value, void *closure) {
    const char *const ip_address = PyUnicode_AsUTF8(value);

    if (ip_address == NULL) {
        return -1;
    }

    snprintf(self->wrapped.ip_address, sizeof(self->wrapped.ip_address), "%s", ip_address);

    return 0;
}

static PyObject *udp_connection_config_get_send_port(UdpConnectionConfig *self, PyObject *args) {
    return PyLong_FromUnsignedLong((unsigned long) self->wrapped.send_port);
}

static int udp_connection_config_set_send_port(UdpConnectionConfig *self, PyObject *value, void *closure) {
    const uint16_t send_port = (uint16_t) PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->wrapped.send_port = send_port;

    return 0;
}

static PyObject *udp_connection_config_get_receive_port(UdpConnectionConfig *self, PyObject *args) {
    return PyLong_FromUnsignedLong((unsigned long) self->wrapped.receive_port);
}

static int udp_connection_config_set_receive_port(UdpConnectionConfig *self, PyObject *value, void *closure) {
    const uint16_t receive_port = (uint16_t) PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->wrapped.receive_port = receive_port;

    return 0;
}

static PyGetSetDef udp_connection_config_get_set[] = {
    {"ip_address", (getter) udp_connection_config_get_ip_address, (setter) udp_connection_config_set_ip_address, "", NULL},
    {"send_port", (getter) udp_connection_config_get_send_port, (setter) udp_connection_config_set_send_port, "", NULL},
    {"receive_port", (getter) udp_connection_config_get_receive_port, (setter) udp_connection_config_set_receive_port, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject udp_connection_config_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.UdpConnectionConfig",
    .tp_basicsize = sizeof(UdpConnectionConfig),
    .tp_dealloc = (destructor) udp_connection_config_free,
    .tp_str = (reprfunc) udp_connection_config_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = udp_connection_config_new,
    .tp_getset = udp_connection_config_get_set,
};

static PyObject *udp_connection_config_from(const XIMU3_UdpConnectionConfig *const config) {
    UdpConnectionConfig *const self = (UdpConnectionConfig *) udp_connection_config_object.tp_alloc(&udp_connection_config_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = *config;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// Bluetooth connection config

typedef struct {
    PyObject_HEAD
    XIMU3_BluetoothConnectionConfig wrapped;
} BluetoothConnectionConfig;

static PyObject *bluetooth_connection_config_str(BluetoothConnectionConfig *self) {
    const char *const string = XIMU3_bluetooth_connection_config_to_string(self->wrapped);

    return PyUnicode_FromString(string);
}

static PyObject *bluetooth_connection_config_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *port_name;

    static char *kwlist[] = {
        "port_name",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &port_name) == 0) {
        return NULL;
    }

    BluetoothConnectionConfig *const self = (BluetoothConnectionConfig *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->wrapped.port_name, sizeof(self->wrapped.port_name), "%s", port_name);

    return (PyObject *) self;
}

static void bluetooth_connection_config_free(BluetoothConnectionConfig *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *bluetooth_connection_config_get_port_name(BluetoothConnectionConfig *self, PyObject *args) {
    return PyUnicode_FromString(self->wrapped.port_name);
}

static int bluetooth_connection_config_set_port_name(BluetoothConnectionConfig *self, PyObject *value, void *closure) {
    const char *const port_name = PyUnicode_AsUTF8(value);

    if (port_name == NULL) {
        return -1;
    }

    snprintf(self->wrapped.port_name, sizeof(self->wrapped.port_name), "%s", port_name);

    return 0;
}

static PyGetSetDef bluetooth_connection_config_get_set[] = {
    {"port_name", (getter) bluetooth_connection_config_get_port_name, (setter) bluetooth_connection_config_set_port_name, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject bluetooth_connection_config_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.BluetoothConnectionConfig",
    .tp_basicsize = sizeof(BluetoothConnectionConfig),
    .tp_dealloc = (destructor) bluetooth_connection_config_free,
    .tp_str = (reprfunc) bluetooth_connection_config_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = bluetooth_connection_config_new,
    .tp_getset = bluetooth_connection_config_get_set,
};

static PyObject *bluetooth_connection_config_from(const XIMU3_BluetoothConnectionConfig *const config) {
    BluetoothConnectionConfig *const self = (BluetoothConnectionConfig *) bluetooth_connection_config_object.tp_alloc(&bluetooth_connection_config_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = *config;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// File connection config

typedef struct {
    PyObject_HEAD
    XIMU3_FileConnectionConfig wrapped;
} FileConnectionConfig;

static PyObject *file_connection_config_str(FileConnectionConfig *self) {
    const char *const string = XIMU3_file_connection_config_to_string(self->wrapped);

    return PyUnicode_FromString(string);
}

static PyObject *file_connection_config_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *file_path;

    static char *kwlist[] = {
        "file_path",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &file_path) == 0) {
        return NULL;
    }

    FileConnectionConfig *const self = (FileConnectionConfig *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->wrapped.file_path, sizeof(self->wrapped.file_path), "%s", file_path);

    return (PyObject *) self;
}

static void file_connection_config_free(FileConnectionConfig *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *file_connection_config_get_file_path(FileConnectionConfig *self, PyObject *args) {
    return PyUnicode_FromString(self->wrapped.file_path);
}

static PyGetSetDef file_connection_config_get_set[] = {
    {"file_path", (getter) file_connection_config_get_file_path, NULL, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject file_connection_config_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.FileConnectionConfig",
    .tp_basicsize = sizeof(FileConnectionConfig),
    .tp_dealloc = (destructor) file_connection_config_free,
    .tp_str = (reprfunc) file_connection_config_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = file_connection_config_new,
    .tp_getset = file_connection_config_get_set,
};

static PyObject *file_connection_config_from(const XIMU3_FileConnectionConfig *const config) {
    FileConnectionConfig *const self = (FileConnectionConfig *) file_connection_config_object.tp_alloc(&file_connection_config_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = *config;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// Mux connection config

typedef struct {
    PyObject_HEAD
    XIMU3_MuxConnectionConfig *wrapped;
} MuxConnectionConfig;

PyObject *mux_connection_config_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

static void mux_connection_config_free(MuxConnectionConfig *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_mux_connection_config_free(self->wrapped);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *mux_connection_config_str(MuxConnectionConfig *self) {
    const char *const string = XIMU3_mux_connection_config_to_string(self->wrapped);

    return PyUnicode_FromString(string);
}

static PyTypeObject mux_connection_config_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.MuxConnectionConfig",
    .tp_basicsize = sizeof(MuxConnectionConfig),
    .tp_dealloc = (destructor) mux_connection_config_free,
    .tp_str = (reprfunc) mux_connection_config_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = mux_connection_config_new,
};

static PyObject *mux_connection_config_from(XIMU3_MuxConnectionConfig *const config) {
    MuxConnectionConfig *const self = (MuxConnectionConfig *) mux_connection_config_object.tp_alloc(&mux_connection_config_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = config;
    return (PyObject *) self;
}

#endif
