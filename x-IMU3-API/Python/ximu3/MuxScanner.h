#ifndef MUX_SCANNER_H
#define MUX_SCANNER_H

#include "../../C/Ximu3.h"
#include "Device.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_MuxScanner *wrapped;
} MuxScanner;

static PyObject *mux_scanner_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    PyObject *connection;
    PyObject *callback;

    static char *kwlist[] = {
        "connection",
        "callback",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!O", kwlist, &connection_object, &connection, &callback) == 0) {
        return NULL;
    }

    if (PyCallable_Check(callback) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    MuxScanner *const self = (MuxScanner *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    Py_INCREF(callback); // TODO: this will never be destroyed (memory leak)

    self->wrapped = XIMU3_mux_scanner_new(((Connection *) connection)->wrapped, devices_callback, callback);
    return (PyObject *) self;
}

static void mux_scanner_free(MuxScanner *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_mux_scanner_free(self->wrapped);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *mux_scanner_get_devices(MuxScanner *self, PyObject *args) {
    const XIMU3_Devices devices = XIMU3_mux_scanner_get_devices(self->wrapped);

    return devices_to_list_and_free(devices);
}

static PyObject *mux_scanner_scan(PyObject *null, PyObject *args, PyObject *kwds) {
    PyObject *connection;
    unsigned long number_of_channels;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    static char *kwlist[] = {
        "connection",
        "number_of_channels",
        "retries",
        "timeout",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!k|kk", kwlist, &connection_object, &connection, &number_of_channels, &retries, &timeout) == 0) {
        return NULL;
    }

    const XIMU3_Devices devices = XIMU3_mux_scanner_scan(((Connection *) connection)->wrapped, number_of_channels, retries, timeout);

    return devices_to_list_and_free(devices);
}

static PyMethodDef mux_scanner_methods[] = {
    {"get_devices", (PyCFunction) mux_scanner_get_devices, METH_NOARGS, ""},
    {"scan", (PyCFunction) mux_scanner_scan, METH_VARARGS | METH_KEYWORDS | METH_STATIC, ""},
    {NULL} /* sentinel */
};

static PyTypeObject mux_scanner_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.MuxScanner",
    .tp_basicsize = sizeof(MuxScanner),
    .tp_dealloc = (destructor) mux_scanner_free,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = mux_scanner_new,
    .tp_methods = mux_scanner_methods,
};

#endif
