// This file was generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#ifndef SERIAL_ACCESSORY_MESSAGE_H
#define SERIAL_ACCESSORY_MESSAGE_H

#include "../../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_SerialAccessoryMessage message;
} SerialAccessoryMessage;

static void serial_accessory_message_free(SerialAccessoryMessage* self)
{
    Py_TYPE(self)->tp_free(self);
}

PyObject* serial_accessory_message_get_timestamp(SerialAccessoryMessage* self, PyObject* args)
{
    return Py_BuildValue("K", self->message.timestamp);
}

PyObject* serial_accessory_message_get_string(SerialAccessoryMessage* self, PyObject* args)
{
    return Py_BuildValue("s", self->message.char_array);
}

PyObject* serial_accessory_message_get_bytes(SerialAccessoryMessage* self, PyObject* args)
{
    return PyByteArray_FromStringAndSize(self->message.char_array, self->message.number_of_bytes);
}

static PyObject* serial_accessory_message_to_string(SerialAccessoryMessage* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_serial_accessory_message_to_string(self->message));
}

static PyGetSetDef serial_accessory_message_get_set[] = {
        { "timestamp", (getter) serial_accessory_message_get_timestamp, NULL, "", NULL },
        { "string",    (getter) serial_accessory_message_get_string,    NULL, "", NULL },
        { "bytes",     (getter) serial_accessory_message_get_bytes,     NULL, "", NULL },
        { NULL }  /* sentinel */
};

static PyMethodDef serial_accessory_message_methods[] = {
        { "to_string", (PyCFunction) serial_accessory_message_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject serial_accessory_message_type = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.SerialAccessoryMessage",
        .tp_basicsize = sizeof(SerialAccessoryMessage),
        .tp_dealloc = (destructor) serial_accessory_message_free,
        .tp_getset = serial_accessory_message_get_set,
        .tp_methods = serial_accessory_message_methods,
};

static PyObject* serial_accessory_message_from(const XIMU3_SerialAccessoryMessage* const message)
{
    SerialAccessoryMessage* const self = (SerialAccessoryMessage*) serial_accessory_message_type.tp_alloc(&serial_accessory_message_type, 0);
    self->message = *message;
    return (PyObject*) self;
}

typedef struct
{
    PyObject* callable;
    XIMU3_SerialAccessoryMessage data;
} SerialAccessoryPendingCallArg;

static int serial_accessory_message_pending_call_func(void* arg)
{
    PyObject* const object = serial_accessory_message_from(&((SerialAccessoryPendingCallArg*) arg)->data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject(((SerialAccessoryPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);
    free(arg);
    return 0;
}

static void serial_accessory_message_callback(XIMU3_SerialAccessoryMessage data, void* context)
{
    SerialAccessoryPendingCallArg* const arg = malloc(sizeof(SerialAccessoryPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = data;
    Py_AddPendingCall(&serial_accessory_message_pending_call_func, arg);
}

#endif
