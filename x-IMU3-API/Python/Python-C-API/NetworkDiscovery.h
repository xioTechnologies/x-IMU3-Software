#ifndef NETWORK_DISCOVERY_H
#define NETWORK_DISCOVERY_H

#include "../../C/Ximu3.h"
#include "DiscoveredNetworkDevice.h"
#include "Helpers.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_NetworkDiscovery* discovery;
} NetworkDiscovery;

static PyObject* network_discovery_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
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

    NetworkDiscovery* const self = (NetworkDiscovery*) subtype->tp_alloc(subtype, 0);
    self->discovery = XIMU3_network_discovery_new(discovered_network_devices_callback, callable);
    return (PyObject*) self;
}

static void network_discovery_free(NetworkDiscovery* self)
{
    XIMU3_network_discovery_free(self->discovery);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* network_discovery_get_devices(NetworkDiscovery* self, PyObject* args)
{
    return discovered_network_devices_to_list_and_free(XIMU3_network_discovery_get_devices(self->discovery));
}

static PyObject* network_discovery_scan(PyObject* null, PyObject* args)
{
    unsigned long milliseconds;

    if (PyArg_ParseTuple(args, "k", &milliseconds) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    return discovered_network_devices_to_list_and_free(XIMU3_network_discovery_scan((uint32_t) milliseconds));
}

static PyMethodDef network_discovery_methods[] = {
        { "get_devices", (PyCFunction) network_discovery_get_devices, METH_NOARGS, "" },
        { "scan",        (PyCFunction) network_discovery_scan,        METH_VARARGS | METH_STATIC, "" },
        { NULL } /* sentinel */
};

static PyTypeObject network_discovery_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.NetworkDiscovery",
        .tp_basicsize = sizeof(NetworkDiscovery),
        .tp_dealloc = (destructor) network_discovery_free,
        .tp_new = network_discovery_new,
        .tp_methods = network_discovery_methods,
};

#endif
