// This file was generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#ifndef TEMPERATURE_MESSAGE_H
#define TEMPERATURE_MESSAGE_H

#include "../../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_TemperatureMessage message;
} TemperatureMessage;

static void temperature_message_free(TemperatureMessage* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* temperature_message_get_timestamp(TemperatureMessage* self)
{
    return Py_BuildValue("K", self->message.timestamp);
}

static PyObject* temperature_message_get_temperature(TemperatureMessage* self)
{
    return Py_BuildValue("f", self->message.temperature);
}

static PyObject* temperature_message_to_string(TemperatureMessage* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_temperature_message_to_string(self->message));
}

static PyGetSetDef temperature_message_get_set[] = {
        { "timestamp",   (getter) temperature_message_get_timestamp,   NULL, "", NULL },
        { "temperature", (getter) temperature_message_get_temperature, NULL, "", NULL },
        { NULL } /* sentinel */
};

static PyMethodDef temperature_message_methods[] = {
        { "to_string", (PyCFunction) temperature_message_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject temperature_message_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.TemperatureMessage",
        .tp_basicsize = sizeof(TemperatureMessage),
        .tp_dealloc = (destructor) temperature_message_free,
        .tp_getset = temperature_message_get_set,
        .tp_methods = temperature_message_methods,
};

static PyObject* temperature_message_from(const XIMU3_TemperatureMessage* const message)
{
    TemperatureMessage* const self = (TemperatureMessage*) temperature_message_object.tp_alloc(&temperature_message_object, 0);
    self->message = *message;
    return (PyObject*) self;
}

static void temperature_message_callback(XIMU3_TemperatureMessage data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const object = temperature_message_from(&data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

#endif
