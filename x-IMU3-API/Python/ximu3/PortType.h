#ifndef PORT_TYPE_H
#define PORT_TYPE_H

#include "../../C/Ximu3.h"
#include <Python.h>

static int port_type_from(XIMU3_PortType *const port_type, const int port_type_int) {
    switch (port_type_int) {
        case XIMU3_PortTypeUsb:
        case XIMU3_PortTypeSerial:
        case XIMU3_PortTypeBluetooth:
            *port_type = (XIMU3_PortType) port_type_int;
            return 0;
    }

    PyErr_SetString(PyExc_ValueError, "'port_type' must be ximu3.PORT_TYPE_*");
    return -1;
}

static PyObject *port_type_to_string(PyObject *null, PyObject *arg) {
    const int port_type_int =  (int) PyLong_AsLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    XIMU3_PortType port_type;

    if (port_type_from(&port_type, port_type_int) != 0) {
        return NULL;
    }

    const char *const string = XIMU3_port_type_to_string(port_type);

    return PyUnicode_FromString(string);
}

static PyMethodDef port_type_methods[] = {
    {"port_type_to_string", (PyCFunction) port_type_to_string, METH_O, ""},
    {NULL} /* sentinel */
};

#endif
