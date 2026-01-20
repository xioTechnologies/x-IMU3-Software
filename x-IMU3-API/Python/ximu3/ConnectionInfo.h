#ifndef CONNECTION_INFO_H
#define CONNECTION_INFO_H

#include "../../C/Ximu3.h"
#include <Python.h>

//----------------------------------------------------------------------------------------------------------------------
// USB connection info

typedef struct {
    PyObject_HEAD
    XIMU3_UsbConnectionInfo connection_info;
} UsbConnectionInfo;

static PyObject *usb_connection_info_str(UsbConnectionInfo *self) {
    const char *const string = XIMU3_usb_connection_info_to_string(self->connection_info);

    return PyUnicode_FromString(string);
}

static PyObject *usb_connection_info_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *port_name;

    static char *kwlist[] = {
        "port_name",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &port_name) == 0) {
        return NULL;
    }

    UsbConnectionInfo *const self = (UsbConnectionInfo *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->connection_info.port_name, sizeof(self->connection_info.port_name), "%s", port_name);

    return (PyObject *) self;
}

static void usb_connection_info_free(UsbConnectionInfo *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *usb_connection_info_get_port_name(UsbConnectionInfo *self, PyObject *args) {
    return PyUnicode_FromString(self->connection_info.port_name);
}

static int usb_connection_info_set_port_name(UsbConnectionInfo *self, PyObject *value, void *closure) {
    const char *const port_name = PyUnicode_AsUTF8(value);

    if (port_name == NULL) {
        return -1;
    }

    snprintf(self->connection_info.port_name, sizeof(self->connection_info.port_name), "%s", port_name);

    return 0;
}

static PyGetSetDef usb_connection_info_get_set[] = {
    {"port_name", (getter) usb_connection_info_get_port_name, (setter) usb_connection_info_set_port_name, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject usb_connection_info_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.UsbConnectionInfo",
    .tp_basicsize = sizeof(UsbConnectionInfo),
    .tp_dealloc = (destructor) usb_connection_info_free,
    .tp_str = (reprfunc) usb_connection_info_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = usb_connection_info_new,
    .tp_getset = usb_connection_info_get_set,
};

static PyObject *usb_connection_info_from(const XIMU3_UsbConnectionInfo *const connection_info) {
    UsbConnectionInfo *const self = (UsbConnectionInfo *) usb_connection_info_object.tp_alloc(&usb_connection_info_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->connection_info = *connection_info;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// Serial connection info

typedef struct {
    PyObject_HEAD
    XIMU3_SerialConnectionInfo connection_info;
} SerialConnectionInfo;

static PyObject *serial_connection_info_str(SerialConnectionInfo *self) {
    const char *const string = XIMU3_serial_connection_info_to_string(self->connection_info);

    return PyUnicode_FromString(string);
}

static PyObject *serial_connection_info_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
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

    SerialConnectionInfo *const self = (SerialConnectionInfo *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->connection_info.port_name, sizeof(self->connection_info.port_name), "%s", port_name);
    self->connection_info.baud_rate = (uint32_t) baud_rate;
    self->connection_info.rts_cts_enabled = (bool) rts_cts_enabled;

    return (PyObject *) self;
}

static void serial_connection_info_free(SerialConnectionInfo *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *serial_connection_info_get_port_name(SerialConnectionInfo *self, PyObject *args) {
    return PyUnicode_FromString(self->connection_info.port_name);
}

static int serial_connection_info_set_port_name(SerialConnectionInfo *self, PyObject *value, void *closure) {
    const char *const port_name = PyUnicode_AsUTF8(value);

    if (port_name == NULL) {
        return -1;
    }

    snprintf(self->connection_info.port_name, sizeof(self->connection_info.port_name), "%s", port_name);

    return 0;
}

static PyObject *serial_connection_info_get_baud_rate(SerialConnectionInfo *self, PyObject *args) {
    return PyLong_FromUnsignedLong((unsigned long) self->connection_info.baud_rate);
}

static int serial_connection_info_set_baud_rate(SerialConnectionInfo *self, PyObject *value, void *closure) {
    const uint32_t baud_rate = (uint32_t) PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->connection_info.baud_rate = baud_rate;

    return 0;
}

static PyObject *serial_connection_info_get_rts_cts_enabled(SerialConnectionInfo *self, PyObject *args) {
    return PyBool_FromLong((long) self->connection_info.rts_cts_enabled);
}

static int serial_connection_info_set_rts_cts_enabled(SerialConnectionInfo *self, PyObject *value, void *closure) {
    const bool rts_cts_enabled = PyObject_IsTrue(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->connection_info.rts_cts_enabled = rts_cts_enabled;

    return 0;
}

static PyGetSetDef serial_connection_info_get_set[] = {
    {"port_name", (getter) serial_connection_info_get_port_name, (setter) serial_connection_info_set_port_name, "", NULL},
    {"baud_rate", (getter) serial_connection_info_get_baud_rate, (setter) serial_connection_info_set_baud_rate, "", NULL},
    {"rts_cts_enabled", (getter) serial_connection_info_get_rts_cts_enabled, (setter) serial_connection_info_set_rts_cts_enabled, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject serial_connection_info_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.SerialConnectionInfo",
    .tp_basicsize = sizeof(SerialConnectionInfo),
    .tp_dealloc = (destructor) serial_connection_info_free,
    .tp_str = (reprfunc) serial_connection_info_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = serial_connection_info_new,
    .tp_getset = serial_connection_info_get_set,
};

static PyObject *serial_connection_info_from(const XIMU3_SerialConnectionInfo *const connection_info) {
    SerialConnectionInfo *const self = (SerialConnectionInfo *) serial_connection_info_object.tp_alloc(&serial_connection_info_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->connection_info = *connection_info;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// TCP connection info

typedef struct {
    PyObject_HEAD
    XIMU3_TcpConnectionInfo connection_info;
} TcpConnectionInfo;

static PyObject *tcp_connection_info_str(TcpConnectionInfo *self) {
    const char *const string = XIMU3_tcp_connection_info_to_string(self->connection_info);

    return PyUnicode_FromString(string);
}

static PyObject *tcp_connection_info_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
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

    TcpConnectionInfo *const self = (TcpConnectionInfo *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->connection_info.ip_address, sizeof(self->connection_info.ip_address), "%s", ip_address);
    self->connection_info.port = (uint16_t) port;

    return (PyObject *) self;
}

static void tcp_connection_info_free(TcpConnectionInfo *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *tcp_connection_info_get_ip_address(TcpConnectionInfo *self, PyObject *args) {
    return PyUnicode_FromString(self->connection_info.ip_address);
}

static int tcp_connection_info_set_ip_address(TcpConnectionInfo *self, PyObject *value, void *closure) {
    const char *const ip_address = PyUnicode_AsUTF8(value);

    if (ip_address == NULL) {
        return -1;
    }

    snprintf(self->connection_info.ip_address, sizeof(self->connection_info.ip_address), "%s", ip_address);

    return 0;
}

static PyObject *tcp_connection_info_get_port(TcpConnectionInfo *self, PyObject *args) {
    return PyLong_FromUnsignedLong((unsigned long) self->connection_info.port);
}

static int tcp_connection_info_set_port(TcpConnectionInfo *self, PyObject *value, void *closure) {
    const uint16_t port = (uint16_t) PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->connection_info.port = port;

    return 0;
}

static PyGetSetDef tcp_connection_info_get_set[] = {
    {"ip_address", (getter) tcp_connection_info_get_ip_address, (setter) tcp_connection_info_set_ip_address, "", NULL},
    {"port", (getter) tcp_connection_info_get_port, (setter) tcp_connection_info_set_port, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject tcp_connection_info_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.TcpConnectionInfo",
    .tp_basicsize = sizeof(TcpConnectionInfo),
    .tp_dealloc = (destructor) tcp_connection_info_free,
    .tp_str = (reprfunc) tcp_connection_info_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = tcp_connection_info_new,
    .tp_getset = tcp_connection_info_get_set,
};

static PyObject *tcp_connection_info_from(const XIMU3_TcpConnectionInfo *const connection_info) {
    TcpConnectionInfo *const self = (TcpConnectionInfo *) tcp_connection_info_object.tp_alloc(&tcp_connection_info_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->connection_info = *connection_info;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// UDP connection info

typedef struct {
    PyObject_HEAD
    XIMU3_UdpConnectionInfo connection_info;
} UdpConnectionInfo;

static PyObject *udp_connection_info_str(UdpConnectionInfo *self) {
    const char *const string = XIMU3_udp_connection_info_to_string(self->connection_info);

    return PyUnicode_FromString(string);
}

static PyObject *udp_connection_info_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
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

    UdpConnectionInfo *const self = (UdpConnectionInfo *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->connection_info.ip_address, sizeof(self->connection_info.ip_address), "%s", ip_address);
    self->connection_info.send_port = (uint16_t) send_port;
    self->connection_info.receive_port = (uint16_t) receive_port;

    return (PyObject *) self;
}

static void udp_connection_info_free(UdpConnectionInfo *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *udp_connection_info_get_ip_address(UdpConnectionInfo *self, PyObject *args) {
    return PyUnicode_FromString(self->connection_info.ip_address);
}

static int udp_connection_info_set_ip_address(UdpConnectionInfo *self, PyObject *value, void *closure) {
    const char *const ip_address = PyUnicode_AsUTF8(value);

    if (ip_address == NULL) {
        return -1;
    }

    snprintf(self->connection_info.ip_address, sizeof(self->connection_info.ip_address), "%s", ip_address);

    return 0;
}

static PyObject *udp_connection_info_get_send_port(UdpConnectionInfo *self, PyObject *args) {
    return PyLong_FromUnsignedLong((unsigned long) self->connection_info.send_port);
}

static int udp_connection_info_set_send_port(UdpConnectionInfo *self, PyObject *value, void *closure) {
    const uint16_t send_port = (uint16_t) PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->connection_info.send_port = send_port;

    return 0;
}

static PyObject *udp_connection_info_get_receive_port(UdpConnectionInfo *self, PyObject *args) {
    return PyLong_FromUnsignedLong((unsigned long) self->connection_info.receive_port);
}

static int udp_connection_info_set_receive_port(UdpConnectionInfo *self, PyObject *value, void *closure) {
    const uint16_t receive_port = (uint16_t) PyLong_AsUnsignedLong(value);

    if (PyErr_Occurred()) {
        return -1;
    }

    self->connection_info.receive_port = receive_port;

    return 0;
}

static PyGetSetDef udp_connection_info_get_set[] = {
    {"ip_address", (getter) udp_connection_info_get_ip_address, (setter) udp_connection_info_set_ip_address, "", NULL},
    {"send_port", (getter) udp_connection_info_get_send_port, (setter) udp_connection_info_set_send_port, "", NULL},
    {"receive_port", (getter) udp_connection_info_get_receive_port, (setter) udp_connection_info_set_receive_port, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject udp_connection_info_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.UdpConnectionInfo",
    .tp_basicsize = sizeof(UdpConnectionInfo),
    .tp_dealloc = (destructor) udp_connection_info_free,
    .tp_str = (reprfunc) udp_connection_info_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = udp_connection_info_new,
    .tp_getset = udp_connection_info_get_set,
};

static PyObject *udp_connection_info_from(const XIMU3_UdpConnectionInfo *const connection_info) {
    UdpConnectionInfo *const self = (UdpConnectionInfo *) udp_connection_info_object.tp_alloc(&udp_connection_info_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->connection_info = *connection_info;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// Bluetooth connection info

typedef struct {
    PyObject_HEAD
    XIMU3_BluetoothConnectionInfo connection_info;
} BluetoothConnectionInfo;

static PyObject *bluetooth_connection_info_str(BluetoothConnectionInfo *self) {
    const char *const string = XIMU3_bluetooth_connection_info_to_string(self->connection_info);

    return PyUnicode_FromString(string);
}

static PyObject *bluetooth_connection_info_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *port_name;

    static char *kwlist[] = {
        "port_name",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &port_name) == 0) {
        return NULL;
    }

    BluetoothConnectionInfo *const self = (BluetoothConnectionInfo *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->connection_info.port_name, sizeof(self->connection_info.port_name), "%s", port_name);

    return (PyObject *) self;
}

static void bluetooth_connection_info_free(BluetoothConnectionInfo *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *bluetooth_connection_info_get_port_name(BluetoothConnectionInfo *self, PyObject *args) {
    return PyUnicode_FromString(self->connection_info.port_name);
}

static int bluetooth_connection_info_set_port_name(BluetoothConnectionInfo *self, PyObject *value, void *closure) {
    const char *const port_name = PyUnicode_AsUTF8(value);

    if (port_name == NULL) {
        return -1;
    }

    snprintf(self->connection_info.port_name, sizeof(self->connection_info.port_name), "%s", port_name);

    return 0;
}

static PyGetSetDef bluetooth_connection_info_get_set[] = {
    {"port_name", (getter) bluetooth_connection_info_get_port_name, (setter) bluetooth_connection_info_set_port_name, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject bluetooth_connection_info_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.BluetoothConnectionInfo",
    .tp_basicsize = sizeof(BluetoothConnectionInfo),
    .tp_dealloc = (destructor) bluetooth_connection_info_free,
    .tp_str = (reprfunc) bluetooth_connection_info_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = bluetooth_connection_info_new,
    .tp_getset = bluetooth_connection_info_get_set,
};

static PyObject *bluetooth_connection_info_from(const XIMU3_BluetoothConnectionInfo *const connection_info) {
    BluetoothConnectionInfo *const self = (BluetoothConnectionInfo *) bluetooth_connection_info_object.tp_alloc(&bluetooth_connection_info_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->connection_info = *connection_info;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// File connection info

typedef struct {
    PyObject_HEAD
    XIMU3_FileConnectionInfo connection_info;
} FileConnectionInfo;

static PyObject *file_connection_info_str(FileConnectionInfo *self) {
    const char *const string = XIMU3_file_connection_info_to_string(self->connection_info);

    return PyUnicode_FromString(string);
}

static PyObject *file_connection_info_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *file_path;

    static char *kwlist[] = {
        "file_path",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "s", kwlist, &file_path) == 0) {
        return NULL;
    }

    FileConnectionInfo *const self = (FileConnectionInfo *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    snprintf(self->connection_info.file_path, sizeof(self->connection_info.file_path), "%s", file_path);

    return (PyObject *) self;
}

static void file_connection_info_free(FileConnectionInfo *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *file_connection_info_get_file_path(FileConnectionInfo *self, PyObject *args) {
    return PyUnicode_FromString(self->connection_info.file_path);
}

static PyGetSetDef file_connection_info_get_set[] = {
    {"file_path", (getter) file_connection_info_get_file_path, NULL, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject file_connection_info_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.FileConnectionInfo",
    .tp_basicsize = sizeof(FileConnectionInfo),
    .tp_dealloc = (destructor) file_connection_info_free,
    .tp_str = (reprfunc) file_connection_info_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = file_connection_info_new,
    .tp_getset = file_connection_info_get_set,
};

static PyObject *file_connection_info_from(const XIMU3_FileConnectionInfo *const connection_info) {
    FileConnectionInfo *const self = (FileConnectionInfo *) file_connection_info_object.tp_alloc(&file_connection_info_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->connection_info = *connection_info;
    return (PyObject *) self;
}

//----------------------------------------------------------------------------------------------------------------------
// Mux connection info

typedef struct {
    PyObject_HEAD
    XIMU3_MuxConnectionInfo *connection_info;
} MuxConnectionInfo;

PyObject *mux_connection_info_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds);

static void mux_connection_info_free(MuxConnectionInfo *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_mux_connection_info_free(self->connection_info);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *mux_connection_info_str(MuxConnectionInfo *self) {
    const char *const string = XIMU3_mux_connection_info_to_string(self->connection_info);

    return PyUnicode_FromString(string);
}

static PyTypeObject mux_connection_info_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.MuxConnectionInfo",
    .tp_basicsize = sizeof(MuxConnectionInfo),
    .tp_dealloc = (destructor) mux_connection_info_free,
    .tp_str = (reprfunc) mux_connection_info_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = mux_connection_info_new,
};

static PyObject *mux_connection_info_from(XIMU3_MuxConnectionInfo *const connection_info) {
    MuxConnectionInfo *const self = (MuxConnectionInfo *) mux_connection_info_object.tp_alloc(&mux_connection_info_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->connection_info = connection_info;
    return (PyObject *) self;
}

#endif
