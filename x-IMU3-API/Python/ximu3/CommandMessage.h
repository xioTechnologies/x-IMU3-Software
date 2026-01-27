#ifndef COMMAND_MESSAGE_H
#define COMMAND_MESSAGE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_CommandMessage wrapped;
} CommandMessage;

static void command_message_free(CommandMessage *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *command_message_get_json(CommandMessage *self) {
    return PyUnicode_FromString(self->wrapped.json);
}

static PyObject *command_message_get_key(CommandMessage *self) {
    return PyUnicode_FromString(self->wrapped.key);
}

static PyObject *command_message_get_value(CommandMessage *self) {
    return PyUnicode_FromString(self->wrapped.value);
}

static PyObject *command_message_get_error(CommandMessage *self) {
    return PyUnicode_FromString(self->wrapped.error);
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
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = command_message_get_set,
};

static PyObject *command_message_from(const XIMU3_CommandMessage *const message) {
    if (strlen(message->json) == 0) {
        Py_RETURN_NONE;
    }

    CommandMessage *const self = (CommandMessage *) command_message_object.tp_alloc(&command_message_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = *message;
    return (PyObject *) self;
}

static PyObject *command_messages_to_list_and_free(const XIMU3_CommandMessages responses) {
    PyObject *list = PyList_New(responses.length);

    if (list == NULL) {
        goto cleanup;
    }

    for (uint32_t index = 0; index < responses.length; index++) {
        PyObject *const item = command_message_from(&responses.array[index]);

        if (item == NULL) {
            Py_DECREF(list);
            list = NULL;
            goto cleanup;
        }

        PyList_SetItem(list, index, item);
    }

cleanup:
    XIMU3_command_messages_free(responses);

    return list;
}

static void command_message_callback(const XIMU3_CommandMessage data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = command_message_from(&data);

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

static void command_messages_callback(const XIMU3_CommandMessages data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = command_messages_to_list_and_free(data);

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
