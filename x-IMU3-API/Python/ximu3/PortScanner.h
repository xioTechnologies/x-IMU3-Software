#ifndef PORT_SCANNER_H
#define PORT_SCANNER_H

#include "../../C/Ximu3.h"
#include "Device.h"
#include "PortType.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_PortScanner *port_scanner;
} PortScanner;

static PyObject *port_scanner_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    PyObject *callback;

    static char *kwlist[] = {
        "callback",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O:set_callback", kwlist, &callback) == 0) {
        return NULL;
    }

    if (PyCallable_Check(callback) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    PortScanner *const self = (PortScanner *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    Py_INCREF(callback); // TODO: this will never be destroyed (memory leak)

    self->port_scanner = XIMU3_port_scanner_new(devices_callback, callback);
    return (PyObject *) self;
}

static void port_scanner_free(PortScanner *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_port_scanner_free(self->port_scanner);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *port_scanner_get_devices(PortScanner *self, PyObject *args) {
    return devices_to_list_and_free(XIMU3_port_scanner_get_devices(self->port_scanner));
}

static PyObject *port_scanner_scan(PyObject *null, PyObject *args) {
    return devices_to_list_and_free(XIMU3_port_scanner_scan());
}

static PyObject *port_scanner_scan_filter(PyObject *null, PyObject *arg) {
    const int port_type_int = (int) PyLong_AsLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    XIMU3_PortType port_type;

    if (port_type_from(&port_type, port_type_int) != 0) {
        return NULL;
    }

    const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(port_type);

    return devices_to_list_and_free(devices);
}

static PyObject *port_scanner_get_port_names(PyObject *null, PyObject *args) {
    const XIMU3_CharArrays char_arrays = XIMU3_port_scanner_get_port_names();

    PyObject *list = PyList_New(char_arrays.length);

    if (list == NULL) {
        goto cleanup;
    }

    for (uint32_t index = 0; index < char_arrays.length; index++) {
        PyObject *const item = PyUnicode_FromString(char_arrays.array[index]);

        if (item == NULL) {
            Py_DECREF(list);
            list = NULL;
            goto cleanup;
        }

        PyList_SetItem(list, index, item);
    }

cleanup:
    XIMU3_char_arrays_free(char_arrays);

    return list;
}

static PyMethodDef port_scanner_methods[] = {
    {"get_devices", (PyCFunction) port_scanner_get_devices, METH_NOARGS, ""},
    {"scan", (PyCFunction) port_scanner_scan, METH_NOARGS | METH_STATIC, ""},
    {"scan_filter", (PyCFunction) port_scanner_scan_filter, METH_O | METH_STATIC, ""},
    {"get_port_names", (PyCFunction) port_scanner_get_port_names, METH_NOARGS | METH_STATIC, ""},
    {NULL} /* sentinel */
};

static PyTypeObject port_scanner_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.PortScanner",
    .tp_basicsize = sizeof(PortScanner),
    .tp_dealloc = (destructor) port_scanner_free,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = port_scanner_new,
    .tp_methods = port_scanner_methods,
};

#endif
