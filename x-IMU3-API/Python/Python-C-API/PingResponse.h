#ifndef PING_RESPONSE_H
#define PING_RESPONSE_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_PingResponse pingResponse;
} PingResponse;

static void ping_response_free(PingResponse* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* ping_response_get_result(PingResponse* self)
{
    return Py_BuildValue("i", self->pingResponse.result);
}

static PyObject* ping_response_get_interface(PingResponse* self)
{
    return Py_BuildValue("s", self->pingResponse.interface);
}

static PyObject* ping_response_get_device_name(PingResponse* self)
{
    return Py_BuildValue("s", self->pingResponse.device_name);
}

static PyObject* ping_response_get_serial_number(PingResponse* self)
{
    return Py_BuildValue("s", self->pingResponse.serial_number);
}

static PyObject* ping_response_to_string(PingResponse* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_ping_response_to_string(self->pingResponse));
}

static PyGetSetDef ping_response_get_set[] = {
        { "result",        (getter) ping_response_get_result,        NULL, "", NULL },
        { "interface",     (getter) ping_response_get_interface,     NULL, "", NULL },
        { "device_name",   (getter) ping_response_get_device_name,   NULL, "", NULL },
        { "serial_number", (getter) ping_response_get_serial_number, NULL, "", NULL },
        { NULL }  /* sentinel */
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

static PyObject* ping_response_from(const XIMU3_PingResponse* const pingResponse)
{
    PingResponse* const self = (PingResponse*) ping_response_object.tp_alloc(&ping_response_object, 0);
    self->pingResponse = *pingResponse;
    return (PyObject*) self;
}

#endif
