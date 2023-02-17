#ifndef CONNECTION_TYPE_H
#define CONNECTION_TYPE_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject* connection_type_to_string(PyObject* self, PyObject* args)
{
    int connection_type_int;

    if (PyArg_ParseTuple(args, "i", &connection_type_int) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const XIMU3_ConnectionType connection_type_enum = (XIMU3_ConnectionType) connection_type_int;

    switch (connection_type_enum)
    {
        case XIMU3_ConnectionTypeUsb:
        case XIMU3_ConnectionTypeSerial:
        case XIMU3_ConnectionTypeTcp:
        case XIMU3_ConnectionTypeUdp:
        case XIMU3_ConnectionTypeBluetooth:
        case XIMU3_ConnectionTypeFile:
            return Py_BuildValue("s", XIMU3_connection_type_to_string(connection_type_enum));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef connection_type_methods[] = {
        { "connection_type_to_string", (PyCFunction) connection_type_to_string, METH_VARARGS, "" },
        { NULL } /* sentinel */
};

#endif
