#ifndef SERIAL_DISCOVERY_H
#define SERIAL_DISCOVERY_H

#include "../../C/Ximu3.h"
#include "DiscoveredSerialDevice.h"
#include "Helpers.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_SerialDiscovery* discovery;
} SerialDiscovery;

static PyObject* serial_discovery_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
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

    SerialDiscovery* const self = (SerialDiscovery*) subtype->tp_alloc(subtype, 0);
    self->discovery = XIMU3_serial_discovery_new(discovered_serial_devices_callback, callable);
    return (PyObject*) self;
}

static void serial_discovery_free(SerialDiscovery* self)
{
    XIMU3_serial_discovery_free(self->discovery);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* serial_discovery_get_devices(SerialDiscovery* self, PyObject* args)
{
    return discovered_serial_devices_to_list_and_free(XIMU3_serial_discovery_get_devices(self->discovery));
}

static PyObject* serial_discovery_scan(PyObject* null, PyObject* args)
{
    unsigned long milliseconds;

    if (PyArg_ParseTuple(args, "k", &milliseconds) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    return discovered_serial_devices_to_list_and_free(XIMU3_serial_discovery_scan((uint32_t) milliseconds));
}

static PyObject* serial_discovery_scan_filter(PyObject* null, PyObject* args)
{
    unsigned long milliseconds;
    char* connection_type_string;

    if (PyArg_ParseTuple(args, "ks", &milliseconds, &connection_type_string) == 0)
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
        return discovered_serial_devices_to_list_and_free(XIMU3_serial_discovery_scan_filter((uint32_t) milliseconds, XIMU3_ConnectionTypeUsb));
    }
    if (strcmp(connection_type_string, "serial") == 0)
    {
        return discovered_serial_devices_to_list_and_free(XIMU3_serial_discovery_scan_filter((uint32_t) milliseconds, XIMU3_ConnectionTypeSerial));
    }
    if (strcmp(connection_type_string, "bluetooth") == 0)
    {
        return discovered_serial_devices_to_list_and_free(XIMU3_serial_discovery_scan_filter((uint32_t) milliseconds, XIMU3_ConnectionTypeBluetooth));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyObject* serial_discovery_get_available_ports(PyObject* self, PyObject* args)
{
    return char_arrays_to_list_and_free(XIMU3_serial_discovery_get_available_ports());
}

static PyMethodDef serial_discovery_methods[] = {
        { "get_devices",         (PyCFunction) serial_discovery_get_devices,         METH_NOARGS, "" },
        { "scan",                (PyCFunction) serial_discovery_scan,                METH_VARARGS | METH_STATIC, "" },
        { "scan_filter",         (PyCFunction) serial_discovery_scan_filter,         METH_VARARGS | METH_STATIC, "" },
        { "get_available_ports", (PyCFunction) serial_discovery_get_available_ports, METH_NOARGS | METH_STATIC,  "" },
        { NULL } /* sentinel */
};

static PyTypeObject serial_discovery_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.SerialDiscovery",
        .tp_basicsize = sizeof(SerialDiscovery),
        .tp_dealloc = (destructor) serial_discovery_free,
        .tp_new = serial_discovery_new,
        .tp_methods = serial_discovery_methods,
};

#endif
