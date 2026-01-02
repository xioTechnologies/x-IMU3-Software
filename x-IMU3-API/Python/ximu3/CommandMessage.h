#ifndef COMMAND_MESSAGE_H
#define COMMAND_MESSAGE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_CommandMessage command_message;
} CommandMessage;

static void command_message_free(CommandMessage *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *command_message_get_json(CommandMessage *self) {
    return Py_BuildValue("s", self->command_message.json);
}

static PyObject *command_message_get_key(CommandMessage *self) {
    return Py_BuildValue("s", self->command_message.key);
}

static PyObject *command_message_get_value(CommandMessage *self) {
    return Py_BuildValue("s", self->command_message.value);
}

static PyObject *command_message_get_error(CommandMessage *self) {
    return Py_BuildValue("s", self->command_message.error);
}

static PyGetSetDef command_message_get_set[] = {
    {"json", (getter) command_message_get_json, NULL, "", NULL},
    {"key", (getter) command_message_get_key, NULL, "", NULL},
    {"value", (getter) command_message_get_value, NULL, "", NULL},
    {"error", (getter) command_message_get_error, NULL, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject command_message_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.CommandMessage",
    .tp_basicsize = sizeof(CommandMessage),
    .tp_dealloc = (destructor) command_message_free,
    .tp_getset = command_message_get_set,
};

static PyObject *command_message_from(const XIMU3_CommandMessage *const message) {
    if (strlen(message->json) == 0) {
        Py_RETURN_NONE;
    }

    CommandMessage *const self = (CommandMessage *) command_message_object.tp_alloc(&command_message_object, 0);
    self->command_message = *message;
    return (PyObject *) self;
}

static PyObject *command_messages_to_list_and_free(const XIMU3_CommandMessages responses) {
    PyObject *const py_object = PyList_New(responses.length);

    for (uint32_t index = 0; index < responses.length; index++) {
        PyList_SetItem(py_object, index, command_message_from(&responses.array[index]));
    }

    XIMU3_command_messages_free(responses);
    return py_object;
}

static void command_message_callback(const XIMU3_CommandMessage data, void *context) {
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject *const object = command_message_from(&data);
    PyObject *const tuple = Py_BuildValue("(O)", object);

    PyObject *const result = PyObject_CallObject((PyObject *) context, tuple);
    if (result == NULL) {
        PyErr_Print();
    }
    Py_XDECREF(result);

    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

static void command_messages_callback(const XIMU3_CommandMessages data, void *context) {
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject *const object = command_messages_to_list_and_free(data);
    PyObject *const tuple = Py_BuildValue("(O)", object);

    PyObject *const result = PyObject_CallObject((PyObject *) context, tuple);
    if (result == NULL) {
        PyErr_Print();
    }
    Py_XDECREF(result);

    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

#endif
