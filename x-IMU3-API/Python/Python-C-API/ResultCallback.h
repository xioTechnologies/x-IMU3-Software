#ifndef RESULT_CALLBACK_H
#define RESULT_CALLBACK_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject* callable;
    XIMU3_Result data;
} ResultCallbackPendingCallArg;

static int result_pending_call_func(void* arg)
{
    PyObject* const tuple = Py_BuildValue("(s)", XIMU3_result_to_string(((ResultCallbackPendingCallArg*) arg)->data));
    Py_DECREF(PyObject_CallObject(((ResultCallbackPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    free(arg);
    return 0;
}

static void result_callback(XIMU3_Result data, void* context)
{
    ResultCallbackPendingCallArg* const arg = malloc(sizeof(ResultCallbackPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = data;
    Py_AddPendingCall(&result_pending_call_func, arg);
}

#endif
