#ifndef CHAR_PTR_ARRAY_CALLBACK_H
#define CHAR_PTR_ARRAY_CALLBACK_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static void char_arrays_callback(XIMU3_CharArrays data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const object = char_arrays_to_list_and_free(data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

#endif
