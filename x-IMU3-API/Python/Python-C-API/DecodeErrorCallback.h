#ifndef DECODE_ERROR_CALLBACK_H
#define DECODE_ERROR_CALLBACK_H

#include "../../C/Ximu3.h"
#include <Python.h>

static void decode_error_callback(XIMU3_DecodeError data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const tuple = Py_BuildValue("(s)", XIMU3_decode_error_to_string(data));
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);

    PyGILState_Release(state);
}

#endif
