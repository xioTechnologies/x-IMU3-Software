#ifndef RECEIVE_ERROR_CALLBACK_H
#define RECEIVE_ERROR_CALLBACK_H

#include "../../C/Ximu3.h"
#include <Python.h>

static PyObject *receive_error_to_string(PyObject *self, PyObject *args) {
    int receive_error_int;

    if (PyArg_ParseTuple(args, "i", &receive_error_int) == 0) {
        return NULL;
    }

    const XIMU3_ReceiveError receive_error = (XIMU3_ReceiveError) receive_error_int;

    switch (receive_error) {
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
            break;
        default:
            PyErr_SetString(PyExc_ValueError, "Expected RECEIVE_ERROR_*");
            return NULL;
    }

    const char *const string = XIMU3_receive_error_to_string(receive_error);

    return PyUnicode_FromString(string);
}

static PyMethodDef receive_error_methods[] = {
    {"receive_error_to_string", (PyCFunction) receive_error_to_string, METH_VARARGS, ""},
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
