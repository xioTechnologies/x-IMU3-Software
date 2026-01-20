#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include "../../C/Ximu3.h"
#include <Python.h>

#define CONNECTIONS_ARRAY_LENGTH 256

typedef struct {
    PyObject_HEAD
    XIMU3_DataLogger *data_logger;
} DataLogger;

static PyObject *data_logger_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *destination;
    const char *name;
    PyObject *connections_sequence;

    static char *kwlist[] = {
        "destination",
        "name",
        "connections",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "ssO", kwlist, &destination, &name, &connections_sequence) == 0) {
        return NULL;
    }

    if (PySequence_Check(connections_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, "'connections' must be a sequence");
        return NULL;
    }

    const uint32_t length = (uint32_t) PySequence_Size(connections_sequence);

    if (length > CONNECTIONS_ARRAY_LENGTH) {
        PyErr_Format(PyExc_ValueError, "'connections' has too many items. Cannot exceed %d.", CONNECTIONS_ARRAY_LENGTH);
        return NULL;
    }

    XIMU3_Connection *connections[CONNECTIONS_ARRAY_LENGTH];

    for (uint32_t index = 0; index < length; index++) {
        PyObject *connection = PySequence_GetItem(connections_sequence, index); // TODO: this will never be destroyed (memory leak)

        if (PyObject_TypeCheck(connection, &connection_object) == 0) {
            PyErr_Format(PyExc_ValueError, "'connections' item must be %s", connection_object.tp_name);
            return NULL;
        }

        connections[index] = ((Connection *) connection)->connection;
    }

    XIMU3_DataLogger *const data_logger = XIMU3_data_logger_new(destination, name, connections, length);

    const XIMU3_Result result = XIMU3_data_logger_get_result(data_logger);

    if (result != XIMU3_ResultOk) {
        const char *const result_string = XIMU3_result_to_string(result);

        PyErr_SetString(PyExc_RuntimeError, result_string);

        XIMU3_data_logger_free(data_logger);
        return NULL;
    }

    DataLogger *const self = (DataLogger *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        XIMU3_data_logger_free(data_logger);
        return NULL;
    }

    self->data_logger = data_logger;
    return (PyObject *) self;
}

static void data_logger_free(DataLogger *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_data_logger_free(self->data_logger);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *data_logger_log(PyObject *null, PyObject *args, PyObject *kwds) {
    const char *destination;
    const char *name;
    PyObject *connections_sequence;
    unsigned long seconds;

    static char *kwlist[] = {
        "destination",
        "name",
        "connections",
        "seconds",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "ssOk", kwlist, &destination, &name, &connections_sequence, &seconds) == 0) {
        return NULL;
    }

    if (PySequence_Check(connections_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, "'connections' must be a sequence");
        return NULL;
    }

    const uint32_t length = (uint32_t) PySequence_Size(connections_sequence);

    if (length > CONNECTIONS_ARRAY_LENGTH) {
        PyErr_Format(PyExc_ValueError, "'connections' has too many items. Cannot exceed %d.", CONNECTIONS_ARRAY_LENGTH);
        return NULL;
    }

    XIMU3_Connection *connections[CONNECTIONS_ARRAY_LENGTH];

    for (uint32_t index = 0; index < length; index++) {
        PyObject *connection = PySequence_GetItem(connections_sequence, index); // TODO: this will never be destroyed (memory leak)

        if (PyObject_TypeCheck(connection, &connection_object) == 0) {
            PyErr_Format(PyExc_ValueError, "'connections' item must be %s", connection_object.tp_name);
            return NULL;
        }

        connections[index] = ((Connection *) connection)->connection;
    }

    const XIMU3_Result result = XIMU3_data_logger_log(destination, name, connections, length, (uint32_t) seconds);

    if (result != XIMU3_ResultOk) {
        const char *const result_string = XIMU3_result_to_string(result);

        PyErr_SetString(PyExc_RuntimeError, result_string);
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef data_logger_methods[] = {
    {"log", (PyCFunction) data_logger_log, METH_VARARGS | METH_KEYWORDS | METH_STATIC, ""},
    {NULL} /* sentinel */
};

static PyTypeObject data_logger_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.DataLogger",
    .tp_basicsize = sizeof(DataLogger),
    .tp_dealloc = (destructor) data_logger_free,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = data_logger_new,
    .tp_methods = data_logger_methods,
};

#endif
