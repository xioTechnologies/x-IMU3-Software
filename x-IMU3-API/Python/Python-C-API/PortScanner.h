#ifndef PORT_SCANNER_H
#define PORT_SCANNER_H

#include "../../C/Ximu3.h"
#include "Device.h"
#include "Helpers.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_PortScanner* port_scanner;
} PortScanner;

static PyObject* port_scanner_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
{
    PyObject* callable;
    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
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

    PortScanner* const self = (PortScanner*) subtype->tp_alloc(subtype, 0);
    self->port_scanner = XIMU3_port_scanner_new(devices_callback, callable);
    return (PyObject*) self;
}

static void port_scanner_free(PortScanner* self)
{
    XIMU3_port_scanner_free(self->port_scanner);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* port_scanner_get_devices(PortScanner* self, PyObject* args)
{
    return devices_to_list_and_free(XIMU3_port_scanner_get_devices(self->port_scanner));
}

static PyObject* port_scanner_scan(PyObject* null, PyObject* args)
{
    return devices_to_list_and_free(XIMU3_port_scanner_scan());
}

static PyObject* port_scanner_scan_filter(PyObject* null, PyObject* args)
{
    char* connection_type_string;

    if (PyArg_ParseTuple(args, "s", &connection_type_string) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    for (unsigned int index = 0; index < strlen(connection_type_string); index++)
    {
        connection_type_string[index] = tolower(connection_type_string[index]);
    }

    if (strcmp(connection_type_string, "usb") == 0)
    {
        return devices_to_list_and_free(XIMU3_port_scanner_scan_filter(XIMU3_ConnectionTypeUsb));
    }
    if (strcmp(connection_type_string, "serial") == 0)
    {
        return devices_to_list_and_free(XIMU3_port_scanner_scan_filter(XIMU3_ConnectionTypeSerial));
    }
    if (strcmp(connection_type_string, "bluetooth") == 0)
    {
        return devices_to_list_and_free(XIMU3_port_scanner_scan_filter(XIMU3_ConnectionTypeBluetooth));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyObject* port_scanner_get_port_names(PyObject* null, PyObject* args)
{
    return char_arrays_to_list_and_free(XIMU3_port_scanner_get_port_names());
}

static PyMethodDef port_scanner_methods[] = {
        { "get_devices",    (PyCFunction) port_scanner_get_devices,    METH_NOARGS, "" },
        { "scan",           (PyCFunction) port_scanner_scan,           METH_NOARGS | METH_STATIC,  "" },
        { "scan_filter",    (PyCFunction) port_scanner_scan_filter,    METH_VARARGS | METH_STATIC, "" },
        { "get_port_names", (PyCFunction) port_scanner_get_port_names, METH_NOARGS | METH_STATIC,  "" },
        { NULL } /* sentinel */
};

static PyTypeObject port_scanner_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.PortScanner",
        .tp_basicsize = sizeof(PortScanner),
        .tp_dealloc = (destructor) port_scanner_free,
        .tp_new = port_scanner_new,
        .tp_methods = port_scanner_methods,
};

#endif
