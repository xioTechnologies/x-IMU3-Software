#ifndef CONNECTION_STATUS_H
#define CONNECTION_STATUS_H

#include "../../C/Ximu3.h"
#include <Python.h>

static int connection_status_from(XIMU3_ConnectionStatus *const connection_status, const int connection_status_int) {
    switch (connection_status_int) {
        case XIMU3_ConnectionStatusConnected:
        case XIMU3_ConnectionStatusReconnecting:
            *connection_status = (XIMU3_ConnectionStatus) connection_status_int;
            return 0;
    }

    PyErr_SetString(PyExc_ValueError, "'connection_status' must be ximu3.CONNECTION_STATUS_*");
    return -1;
}

static PyObject *connection_status_to_string(PyObject *null, PyObject *arg) {
    const int connection_status_int = (int) PyLong_AsLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    XIMU3_ConnectionStatus connection_status;

    if (connection_status_from(&connection_status, connection_status_int) != 0) {
        return NULL;
    }

    const char *const string = XIMU3_connection_status_to_string(connection_status);

    return PyUnicode_FromString(string);
}

static PyMethodDef connection_status_methods[] = {
    {"connection_status_to_string", (PyCFunction) connection_status_to_string, METH_O, ""},
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
