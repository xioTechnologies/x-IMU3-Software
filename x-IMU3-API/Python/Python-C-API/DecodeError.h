#ifndef DECODE_ERROR_CALLBACK_H
#define DECODE_ERROR_CALLBACK_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject* decode_error_to_string(PyObject* self, PyObject* args)
{
    int decode_error_int;

    if (PyArg_ParseTuple(args, "i", &decode_error_int) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const XIMU3_DecodeError decode_error_enum = (XIMU3_DecodeError) decode_error_int;

    switch (decode_error_enum)
    {
        case XIMU3_DecodeErrorBufferOverrun:
        case XIMU3_DecodeErrorInvalidMessageIdentifier:
        case XIMU3_DecodeErrorInvalidUtf8:
        case XIMU3_DecodeErrorInvalidJson:
        case XIMU3_DecodeErrorJsonIsNotAnObject:
        case XIMU3_DecodeErrorJsonObjectIsNotASingleKeyValuePair:
        case XIMU3_DecodeErrorInvalidEscapeSequence:
        case XIMU3_DecodeErrorInvalidBinaryMessageLength:
        case XIMU3_DecodeErrorUnableToParseAsciiMessage:
            return Py_BuildValue("s", XIMU3_decode_error_to_string(decode_error_enum));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef decode_error_methods[] = {
        { "decode_error_to_string", (PyCFunction) decode_error_to_string, METH_VARARGS, "" },
        { NULL } /* sentinel */
};

static void decode_error_callback(XIMU3_DecodeError data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const tuple = Py_BuildValue("(i)", data);
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);

    PyGILState_Release(state);
}

#endif
