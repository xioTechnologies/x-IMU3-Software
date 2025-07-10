#ifndef KEEP_OPEN_H
#define KEEP_OPEN_H

#include "../../C/Ximu3.h"
#include "Connection.h"
#include "ConnectionStatus.h"
#include "Helpers.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_KeepOpen* keep_open;
} KeepOpen;

static PyObject* keep_open_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
{
    PyObject* connection;
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O!O:set_callback", &connection_object, &connection, &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    KeepOpen* const self = (KeepOpen*) subtype->tp_alloc(subtype, 0);
    self->keep_open = XIMU3_keep_open_new(((Connection*) connection)->connection, connection_status_callback, callable);
    return (PyObject*) self;
}

static void keep_open_free(KeepOpen* self)
{
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
    .tp_new = keep_open_new,
};

#endif
