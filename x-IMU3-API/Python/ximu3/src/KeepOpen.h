#ifndef KEEP_OPEN_H
#define KEEP_OPEN_H

#include "../../../C/Ximu3.h"
#include "Connection.h"
#include "ConnectionStatus.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_KeepOpen *wrapped;
} KeepOpen;

static PyObject *keep_open_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    PyObject *connection;

    static char *kwlist[] = {
        "connection",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist, &connection_object, &connection) == 0) {
        return NULL;
    }

    KeepOpen *const self = (KeepOpen *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = XIMU3_keep_open_new(((Connection *) connection)->wrapped);
    return (PyObject *) self;
}

static void keep_open_free(KeepOpen *self) {
    XIMU3_keep_open_free(self->wrapped);
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
