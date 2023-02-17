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
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_port_scanner_free(self->port_scanner);
    Py_END_ALLOW_THREADS
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
        case XIMU3_ConnectionTypeBluetooth:
            return devices_to_list_and_free(XIMU3_port_scanner_scan_filter(connection_type_enum));
        case XIMU3_ConnectionTypeTcp:
        case XIMU3_ConnectionTypeUdp:
        case XIMU3_ConnectionTypeFile:
            break;
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
