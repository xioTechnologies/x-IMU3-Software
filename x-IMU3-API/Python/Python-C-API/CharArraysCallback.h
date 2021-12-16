#ifndef CHAR_PTR_ARRAY_CALLBACK_H
#define CHAR_PTR_ARRAY_CALLBACK_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

typedef struct
{
    PyObject* callable;
    XIMU3_CharArrays data;
} CharPtrArrayCallbackPendingCallArg;

static int char_arrays_pending_call_func(void* arg)
{
    PyObject* const object = char_arrays_to_list_and_free(((CharPtrArrayCallbackPendingCallArg*) arg)->data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject(((CharPtrArrayCallbackPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);
    free(arg);
    return 0;
}

static void char_arrays_callback(XIMU3_CharArrays data, void* context)
{
    CharPtrArrayCallbackPendingCallArg* const arg = malloc(sizeof(CharPtrArrayCallbackPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = data;
    Py_AddPendingCall(&char_arrays_pending_call_func, arg);
}

#endif
