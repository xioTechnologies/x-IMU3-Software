#ifndef RECEIVE_ERROR_CALLBACK_H
#define RECEIVE_ERROR_CALLBACK_H

#include "../../C/Ximu3.h"
#include <Python.h>

static int receive_error_from(XIMU3_ReceiveError *const error, const int error_int) {
    switch (error_int) {
        case XIMU3_ReceiveErrorBufferOverrun:
        case XIMU3_ReceiveErrorInvalidMessageIdentifier:
        case XIMU3_ReceiveErrorInvalidJson:
        case XIMU3_ReceiveErrorJsonIsNotAnObject:
        case XIMU3_ReceiveErrorJsonObjectIsNotASingleKeyValuePair:
        case XIMU3_ReceiveErrorInvalidMuxMessageLength:
        case XIMU3_ReceiveErrorInvalidEscapeSequence:
        case XIMU3_ReceiveErrorInvalidBinaryMessageLength:
        case XIMU3_ReceiveErrorUnableToParseAsciiMessage:
        case XIMU3_ReceiveErrorUnknownError:
            *error = (XIMU3_ReceiveError) error_int;
            return 0;
    }

    PyErr_SetString(PyExc_ValueError, "'receive_error' must be ximu3.RECEIVE_ERROR_*");
    return -1;
}

static PyObject *receive_error_to_string(PyObject *null, PyObject *arg) {
    const int error_int = (int) PyLong_AsLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    XIMU3_ReceiveError error;

    if (receive_error_from(&error, error_int) != 0) {
        return NULL;
    }

    const char *const string = XIMU3_receive_error_to_string(error);

    return PyUnicode_FromString(string);
}

static PyMethodDef receive_error_methods[] = {
    {"receive_error_to_string", (PyCFunction) receive_error_to_string, METH_O, ""},
    {NULL} /* sentinel */
};

static void receive_error_callback(XIMU3_ReceiveError data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = PyLong_FromLong((long) data);

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
