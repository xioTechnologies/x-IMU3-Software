#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

#define CONNECTIONS_ARRAY_LENGTH 256

typedef struct
{
    PyObject_HEAD
    XIMU3_DataLogger* data_logger;
} DataLogger;

static PyObject* data_logger_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
{
    const char* directory;
    const char* name;
    PyObject* connections_list;

    if (PyArg_ParseTuple(args, "ssO!", &directory, &name, &PyList_Type, &connections_list) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    XIMU3_Connection* connections_array[CONNECTIONS_ARRAY_LENGTH];
    const uint32_t length = (uint32_t) PyList_Size(connections_list);

    for (uint32_t index = 0; index < length; index++)
    {
        if (index >= CONNECTIONS_ARRAY_LENGTH)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        PyObject* connection = PyList_GetItem(connections_list, index);

        if (PyObject_IsInstance(connection, (PyObject*) &connection_object) != 1)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        connections_array[index] = ((Connection*) connection)->connection;
    }

    DataLogger* const self = (DataLogger*) subtype->tp_alloc(subtype, 0);
    self->data_logger = XIMU3_data_logger_new(directory, name, connections_array, length);
    return (PyObject*) self;
}

static void data_logger_free(DataLogger* self)
{
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_data_logger_free(self->data_logger);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject* data_logger_get_result(DataLogger* self, PyObject* args)
{
    return Py_BuildValue("i", XIMU3_data_logger_get_result(self->data_logger));
}

static PyObject* data_logger_log(PyObject* null, PyObject* args)
{
    const char* directory;
    const char* name;
    PyObject* connections_list;
    unsigned long seconds;

    if (PyArg_ParseTuple(args, "ssO!k", &directory, &name, &PyList_Type, &connections_list, &seconds) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    XIMU3_Connection* connections_array[CONNECTIONS_ARRAY_LENGTH];
    const uint32_t length = (uint32_t) PyList_Size(connections_list);

    for (uint32_t index = 0; index < length; index++)
    {
        if (index >= CONNECTIONS_ARRAY_LENGTH)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        PyObject* connection = PyList_GetItem(connections_list, index);

        if (PyObject_IsInstance(connection, (PyObject*) &connection_object) != 1)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        connections_array[index] = ((Connection*) connection)->connection;
    }

    return Py_BuildValue("i", XIMU3_data_logger_log(directory, name, connections_array, length, (uint32_t) seconds));
}

static PyMethodDef data_logger_methods[] = {
        { "get_result", (PyCFunction) data_logger_get_result, METH_NOARGS, "" },
        { "log",        (PyCFunction) data_logger_log,        METH_VARARGS | METH_STATIC, "" },
        { NULL } /* sentinel */
};

static PyTypeObject data_logger_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.DataLogger",
        .tp_basicsize = sizeof(DataLogger),
        .tp_dealloc = (destructor) data_logger_free,
        .tp_new = data_logger_new,
        .tp_methods = data_logger_methods,
};

#endif
