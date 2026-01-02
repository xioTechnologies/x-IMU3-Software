#ifndef STATISTICS_H
#define STATISTICS_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_Statistics statistics;
} Statistics;

static void statistics_free(Statistics *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *statistics_get_timestamp(Statistics *self) {
    return PyLong_FromUnsignedLongLong((unsigned long long) self->statistics.timestamp);
}

static PyObject *statistics_get_data_total(Statistics *self) {
    return PyLong_FromUnsignedLongLong((unsigned long long) self->statistics.data_total);
}

static PyObject *statistics_get_data_rate(Statistics *self) {
    return PyLong_FromUnsignedLong((unsigned long) self->statistics.data_rate);
}

static PyObject *statistics_get_message_total(Statistics *self) {
    return PyLong_FromUnsignedLongLong((unsigned long long) self->statistics.message_total);
}

static PyObject *statistics_get_message_rate(Statistics *self) {
    return PyLong_FromUnsignedLong((unsigned long) self->statistics.message_rate);
}

static PyObject *statistics_get_error_total(Statistics *self) {
    return PyLong_FromUnsignedLongLong((unsigned long long) self->statistics.error_total);
}

static PyObject *statistics_get_error_rate(Statistics *self) {
    return PyLong_FromUnsignedLong((unsigned long) self->statistics.error_rate);
}

static PyObject *statistics_to_string(Statistics *self, PyObject *args) {
    const char *const string = XIMU3_statistics_to_string(self->statistics);

    return PyUnicode_FromString(string);
}

static PyGetSetDef statistics_get_set[] = {
    {"timestamp", (getter) statistics_get_timestamp, NULL, "", NULL},
    {"data_total", (getter) statistics_get_data_total, NULL, "", NULL},
    {"data_rate", (getter) statistics_get_data_rate, NULL, "", NULL},
    {"message_total", (getter) statistics_get_message_total, NULL, "", NULL},
    {"message_rate", (getter) statistics_get_message_rate, NULL, "", NULL},
    {"error_total", (getter) statistics_get_error_total, NULL, "", NULL},
    {"error_rate", (getter) statistics_get_error_rate, NULL, "", NULL},
    {NULL} /* sentinel */
};

static PyMethodDef statistics_methods[] = {
    {"to_string", (PyCFunction) statistics_to_string, METH_NOARGS, ""},
    {NULL} /* sentinel */
};

static PyTypeObject statistics_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.Statistics",
    .tp_basicsize = sizeof(Statistics),
    .tp_dealloc = (destructor) statistics_free,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = statistics_get_set,
    .tp_methods = statistics_methods,
};

static PyObject *statistics_from(const XIMU3_Statistics *const statistics) {
    Statistics *const self = (Statistics *) statistics_object.tp_alloc(&statistics_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->statistics = *statistics;
    return (PyObject *) self;
}

static void statistics_callback(XIMU3_Statistics data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = statistics_from(&data);

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
