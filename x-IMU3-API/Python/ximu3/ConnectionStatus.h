#ifndef CONNECTION_STATUS_H
#define CONNECTION_STATUS_H

#include "../../C/Ximu3.h"
#include <Python.h>

static PyObject *connection_status_to_string(PyObject *self, PyObject *args) {
    int status_int;

    if (PyArg_ParseTuple(args, "i", &status_int) == 0) {
        return NULL;
    }

    const XIMU3_ConnectionStatus status = (XIMU3_ConnectionStatus) status_int;

    switch (status) {
        case XIMU3_ConnectionStatusConnected:
        case XIMU3_ConnectionStatusReconnecting:
            break;
        default:
            PyErr_SetString(PyExc_ValueError, "Expected CONNECTION_STATUS_*");
            return NULL;
    }

    const char *const string = XIMU3_connection_status_to_string(status);

    return PyUnicode_FromString(string);
}

static PyMethodDef connection_status_methods[] = {
    {"connection_status_to_string", (PyCFunction) connection_status_to_string, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

static void connection_status_callback(XIMU3_ConnectionStatus data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = PyLong_FromLong((long) data);

    if (object == NULL) {
        PyErr_Print();
        goto cleanup;
    }

    tuple = PyTuple_Pack(1, object);

    if (tuple == NULL) {
        PyErr_Print();
        goto cleanup;
    }

    result = PyObject_CallObject((PyObject *) context, tuple);

    if (result == NULL) {
        PyErr_Print();
    }

cleanup:
    Py_XDECREF(object);
    Py_XDECREF(tuple);
    Py_XDECREF(result);

    PyGILState_Release(state);
}

#endif
