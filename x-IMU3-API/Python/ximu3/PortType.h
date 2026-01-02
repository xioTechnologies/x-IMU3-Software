#ifndef PORT_TYPE_H
#define PORT_TYPE_H

#include "../../C/Ximu3.h"
#include <Python.h>

static PyObject *port_type_to_string(PyObject *self, PyObject *args) {
    int port_type_int;

    if (PyArg_ParseTuple(args, "i", &port_type_int) == 0) {
        return NULL;
    }

    const XIMU3_PortType port_type = (XIMU3_PortType) port_type_int;

    switch (port_type) {
        case XIMU3_PortTypeUsb:
        case XIMU3_PortTypeSerial:
        case XIMU3_PortTypeBluetooth:
            break;
        default:
            PyErr_SetString(PyExc_ValueError, "Expected PORT_TYPE_*");
            return NULL;
    }

    const char *const string = XIMU3_port_type_to_string(port_type);

    return PyUnicode_FromString(string);
}

static PyMethodDef port_type_methods[] = {
    {"port_type_to_string", (PyCFunction) port_type_to_string, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

#endif
