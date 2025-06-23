#ifndef DEMUX_H
#define DEMUX_H

#include "../../C/Ximu3.h"
#include "Connection.h"
#include "Helpers.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_Demux* demux;
} Demux;

#define CHANNELS_ARRAY_LENGTH 256

static PyObject* demux_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
{
    PyObject* connection;
    PyObject* channels_sequence;

    if (PyArg_ParseTuple(args, "O!O", &connection_object, &connection, &channels_sequence) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PySequence_Check(channels_sequence) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    uint8_t channels_array[CHANNELS_ARRAY_LENGTH];
    const uint32_t channels_length = (uint32_t) PySequence_Size(channels_sequence);

    for (uint32_t index = 0; index < channels_length; index++)
    {
        if (index >= CHANNELS_ARRAY_LENGTH)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        PyObject* command = PySequence_GetItem(channels_sequence, index);

        if (PyNumber_Check(command) == 0)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        channels_array[index] = (uint8_t) PyLong_AsUnsignedLong(command);
    }

    Demux* self = (Demux*) subtype->tp_alloc(subtype, 0);
    self->demux = XIMU3_demux_new(((Connection*) connection)->connection, channels_array, channels_length);
    return (PyObject*) self;
}

static void demux_free(Demux* self)
{
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_demux_free(self->demux);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject* demux_get_result(Demux* self, PyObject* args)
{
    return Py_BuildValue("i", XIMU3_demux_get_result(self->demux));
}

static PyMethodDef demux_methods[] = {
    { "get_result", (PyCFunction) demux_get_result, METH_NOARGS, "" },
    { NULL } /* sentinel */
};

static PyTypeObject demux_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.Demux",
    .tp_basicsize = sizeof(Demux),
    .tp_dealloc = (destructor) demux_free,
    .tp_new = demux_new,
    .tp_methods = demux_methods,
};

#endif
