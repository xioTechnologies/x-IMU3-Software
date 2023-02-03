#ifndef RESULT_CALLBACK_H
#define RESULT_CALLBACK_H

#include "../../C/Ximu3.h"
#include <Python.h>

static void result_callback(XIMU3_Result data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const tuple = Py_BuildValue("(s)", XIMU3_result_to_string(data));
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);

    PyGILState_Release(state);
}

#endif
