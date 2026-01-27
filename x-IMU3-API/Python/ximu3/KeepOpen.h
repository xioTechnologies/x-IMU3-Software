#ifndef KEEP_OPEN_H
#define KEEP_OPEN_H

#include "../../C/Ximu3.h"
#include "Connection.h"
#include "ConnectionStatus.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_KeepOpen *keep_open;
} KeepOpen;

static PyObject *keep_open_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
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

    KeepOpen *const self = (KeepOpen *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    Py_INCREF(callback); // TODO: this will never be destroyed (memory leak)

    self->keep_open = XIMU3_keep_open_new(((Connection *) connection)->connection, connection_status_callback, callback);
    return (PyObject *) self;
}

static void keep_open_free(KeepOpen *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_keep_open_free(self->keep_open);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyTypeObject keep_open_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.KeepOpen",
    .tp_basicsize = sizeof(KeepOpen),
    .tp_dealloc = (destructor) keep_open_free,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = keep_open_new,
};

#endif
