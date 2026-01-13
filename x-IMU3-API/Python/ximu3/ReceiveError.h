#ifndef RECEIVE_ERROR_CALLBACK_H
#define RECEIVE_ERROR_CALLBACK_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject *receive_error_to_string(PyObject *self, PyObject *args) {
    int receive_error_int;

    if (PyArg_ParseTuple(args, "i", &receive_error_int) == 0) {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const XIMU3_ReceiveError receive_error_enum = (XIMU3_ReceiveError) receive_error_int;

    switch (receive_error_enum) {
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
            return Py_BuildValue("s", XIMU3_receive_error_to_string(receive_error_enum));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef receive_error_methods[] = {
    {"receive_error_to_string", (PyCFunction) receive_error_to_string, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

static void receive_error_callback(XIMU3_ReceiveError data, void *context) {
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject *const tuple = Py_BuildValue("(i)", data);

    PyObject *const result = PyObject_CallObject((PyObject *) context, tuple);
    if (result == NULL) {
        PyErr_Print();
    }
    Py_XDECREF(result);

    Py_DECREF(tuple);

    PyGILState_Release(state);
}

#endif
