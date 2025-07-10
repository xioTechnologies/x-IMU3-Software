#ifndef PORT_TYPE_H
#define PORT_TYPE_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject* port_type_to_string(PyObject* self, PyObject* args)
{
    int port_type_int;

    if (PyArg_ParseTuple(args, "i", &port_type_int) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const XIMU3_PortType port_type_enum = (XIMU3_PortType) port_type_int;

    switch (port_type_enum)
    {
        case XIMU3_PortTypeUsb:
        case XIMU3_PortTypeSerial:
        case XIMU3_PortTypeBluetooth:
            return Py_BuildValue("s", XIMU3_port_type_to_string(port_type_enum));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef port_type_methods[] = {
    { "port_type_to_string", (PyCFunction) port_type_to_string, METH_VARARGS, "" },
    { NULL } /* sentinel */
};

#endif
