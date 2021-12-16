#ifndef DECODE_ERROR_CALLBACK_H
#define DECODE_ERROR_CALLBACK_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject* callable;
    XIMU3_DecodeError data;
} DecodeErrorPendingCallArg;

static int decode_error_pending_call_func(void* arg)
{
    PyObject* const tuple = Py_BuildValue("(s)", XIMU3_decode_error_to_string(((DecodeErrorPendingCallArg*) arg)->data));
    Py_DECREF(PyObject_CallObject(((DecodeErrorPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    free(arg);
    return 0;
}

static void decode_error_callback(XIMU3_DecodeError data, void* context)
{
    DecodeErrorPendingCallArg* const arg = malloc(sizeof(DecodeErrorPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = data;
    Py_AddPendingCall(&decode_error_pending_call_func, arg);
}

#endif
