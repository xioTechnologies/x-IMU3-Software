#ifndef COMMAND_MESSAGE_H
#define COMMAND_MESSAGE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_CommandMessage command_message;
} CommandMessage;

static void command_message_free(CommandMessage* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* command_message_get_json(CommandMessage* self)
{
    return Py_BuildValue("s", self->command_message.json);
}

static PyObject* command_message_get_key(CommandMessage* self)
{
    return Py_BuildValue("s", self->command_message.key);
}

static PyObject* command_message_get_value(CommandMessage* self)
{
    return Py_BuildValue("s", self->command_message.value);
}

static PyObject* command_message_get_error(CommandMessage* self)
{
    return Py_BuildValue("s", self->command_message.error);
}

static PyObject* command_message_parse(PyObject* null, PyObject* args);

static PyGetSetDef command_message_get_set[] = {
    { "json", (getter) command_message_get_json, NULL, "", NULL },
    { "key", (getter) command_message_get_key, NULL, "", NULL },
    { "value", (getter) command_message_get_value, NULL, "", NULL },
    { "error", (getter) command_message_get_error, NULL, "", NULL },
    { NULL } /* sentinel */
};

static PyMethodDef command_message_methods[] = {
    { "parse", (PyCFunction) command_message_parse, METH_VARARGS | METH_STATIC, "" },
    { NULL } /* sentinel */
};

static PyTypeObject command_message_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.CommandMessage",
    .tp_basicsize = sizeof(CommandMessage),
    .tp_dealloc = (destructor) command_message_free,
    .tp_getset = command_message_get_set,
    .tp_methods = command_message_methods,
};

static PyObject* command_message_from(const XIMU3_CommandMessage* const command_message)
{
    CommandMessage* const self = (CommandMessage*) command_message_object.tp_alloc(&command_message_object, 0);
    self->command_message = *command_message;
    return (PyObject*) self;
}

static PyObject* command_message_parse(PyObject* null, PyObject* args)
{
    const char* json;

    if (PyArg_ParseTuple(args, "s", &json) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    XIMU3_CommandMessage command_message = XIMU3_command_message_parse(json);
    return command_message_from(&command_message);
}

#endif
