#ifndef CHAR_PTR_CALLBACK_H
#define CHAR_PTR_CALLBACK_H

#include <Python.h>

typedef struct
{
    PyObject* callable;
    char* data;
} CharPtrCallbackPendingCallArg;

static int char_ptr_pending_call_func(void* arg)
{
    PyObject* const tuple = Py_BuildValue("(s)", ((CharPtrCallbackPendingCallArg*) arg)->data);
    Py_DECREF(PyObject_CallObject(((CharPtrCallbackPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    free(((CharPtrCallbackPendingCallArg*) arg)->data);
    free(arg);
    return 0;
}

static void char_ptr_callback(const char* data, void* context)
{
    CharPtrCallbackPendingCallArg* const arg = malloc(sizeof(CharPtrCallbackPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = malloc(strlen(data) + 1);
    strcpy(arg->data, data);
    Py_AddPendingCall(&char_ptr_pending_call_func, arg);
}

#endif
