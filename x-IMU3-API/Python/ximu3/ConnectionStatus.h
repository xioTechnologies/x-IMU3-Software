#ifndef CONNECTION_STATUS_H
#define CONNECTION_STATUS_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject *connection_status_to_string(PyObject *self, PyObject *args) {
    int connection_status_int;

    if (PyArg_ParseTuple(args, "i", &connection_status_int) == 0) {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const XIMU3_ConnectionStatus connection_status_enum = (XIMU3_ConnectionStatus) connection_status_int;

    switch (connection_status_enum) {
        case XIMU3_ConnectionStatusConnected:
        case XIMU3_ConnectionStatusReconnecting:
            return Py_BuildValue("s", XIMU3_connection_status_to_string(connection_status_enum));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef connection_status_methods[] = {
    {"connection_status_to_string", (PyCFunction) connection_status_to_string, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

static void connection_status_callback(XIMU3_ConnectionStatus data, void *context) {
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject *const tuple = Py_BuildValue("(i)", data);

    PyObject *const result = PyObject_CallObject((PyObject *) context, tuple);
    if (result == NULL) {
        PyErr_Print();
    }
    Py_XDECREF(result);

    Py_DECREF(tuple);

    PyGILState_Release(state);
}

#endif
