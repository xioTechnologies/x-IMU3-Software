#ifndef PING_RESPONSE_H
#define PING_RESPONSE_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_PingResponse ping_response;
} PingResponse;

static void ping_response_free(PingResponse* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* ping_response_get_interface(PingResponse* self)
{
    return Py_BuildValue("s", self->ping_response.interface);
}

static PyObject* ping_response_get_device_name(PingResponse* self)
{
    return Py_BuildValue("s", self->ping_response.device_name);
}

static PyObject* ping_response_get_serial_number(PingResponse* self)
{
    return Py_BuildValue("s", self->ping_response.serial_number);
}

static PyObject* ping_response_to_string(PingResponse* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_ping_response_to_string(self->ping_response));
}

static PyGetSetDef ping_response_get_set[] = {
    { "interface", (getter) ping_response_get_interface, NULL, "", NULL },
    { "device_name", (getter) ping_response_get_device_name, NULL, "", NULL },
    { "serial_number", (getter) ping_response_get_serial_number, NULL, "", NULL },
    { NULL } /* sentinel */
};

static PyMethodDef ping_response_methods[] = {
    { "to_string", (PyCFunction) ping_response_to_string, METH_NOARGS, "" },
    { NULL } /* sentinel */
};

static PyTypeObject ping_response_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.PingResponse",
    .tp_basicsize = sizeof(PingResponse),
    .tp_dealloc = (destructor) ping_response_free,
    .tp_getset = ping_response_get_set,
    .tp_methods = ping_response_methods
};

static PyObject* ping_response_from(const XIMU3_PingResponse* const ping_response)
{
    if (strlen(ping_response->interface) == 0)
    {
        Py_RETURN_NONE;
    }

    PingResponse* const self = (PingResponse*) ping_response_object.tp_alloc(&ping_response_object, 0);
    self->ping_response = *ping_response;
    return (PyObject*) self;
}

static void ping_response_callback(XIMU3_PingResponse data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const object = ping_response_from(&data);
    PyObject* const tuple = Py_BuildValue("(O)", object);

    PyObject* const result = PyObject_CallObject((PyObject*) context, tuple);
    if (result == NULL)
    {
        PyErr_Print();
    }
    Py_XDECREF(result);

    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

#endif
