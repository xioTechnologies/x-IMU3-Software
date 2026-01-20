#ifndef PING_RESPONSE_H
#define PING_RESPONSE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_PingResponse ping_response;
} PingResponse;

static void ping_response_free(PingResponse *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *ping_response_str(PingResponse *self) {
    const char *const string = XIMU3_ping_response_to_string(self->ping_response);

    return PyUnicode_FromString(string);
}

static PyObject *ping_response_get_interface(PingResponse *self) {
    return PyUnicode_FromString(self->ping_response.interface);
}

static PyObject *ping_response_get_device_name(PingResponse *self) {
    return PyUnicode_FromString(self->ping_response.device_name);
}

static PyObject *ping_response_get_serial_number(PingResponse *self) {
    return PyUnicode_FromString(self->ping_response.serial_number);
}

static PyGetSetDef ping_response_get_set[] = {
    {"interface", (getter) ping_response_get_interface, NULL, "", NULL},
    {"device_name", (getter) ping_response_get_device_name, NULL, "", NULL},
    {"serial_number", (getter) ping_response_get_serial_number, NULL, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject ping_response_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.PingResponse",
    .tp_basicsize = sizeof(PingResponse),
    .tp_dealloc = (destructor) ping_response_free,
    .tp_str  = (reprfunc) ping_response_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = ping_response_get_set,
};

static PyObject *ping_response_from(const XIMU3_PingResponse *const ping_response) {
    if (strlen(ping_response->interface) == 0) {
        Py_RETURN_NONE;
    }

    PingResponse *const self = (PingResponse *) ping_response_object.tp_alloc(&ping_response_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->ping_response = *ping_response;
    return (PyObject *) self;
}

static void ping_response_callback(XIMU3_PingResponse data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = ping_response_from(&data);

    if (object == NULL) {
        PyErr_Print();
        goto cleanup;
    }

    tuple = PyTuple_Pack(1, object);

    if (tuple == NULL) {
        PyErr_Print();
        goto cleanup;
    }

    result = PyObject_CallObject((PyObject *) context, tuple);

    if (result == NULL) {
        PyErr_Print();
    }

cleanup:
    Py_XDECREF(object);
    Py_XDECREF(tuple);
    Py_XDECREF(result);

    PyGILState_Release(state);
}

#endif
