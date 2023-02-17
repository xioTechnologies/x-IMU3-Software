#ifndef RESULT_H
#define RESULT_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject* result_to_string(PyObject* self, PyObject* args)
{
    int result_int;

    if (PyArg_ParseTuple(args, "i", &result_int) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const XIMU3_Result result_enum = (XIMU3_Result) result_int;

    switch (result_enum)
    {
        case XIMU3_ResultOk:
        case XIMU3_ResultError:
            return Py_BuildValue("s", XIMU3_result_to_string(result_enum));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef result_methods[] = {
        { "result_to_string", (PyCFunction) result_to_string, METH_VARARGS, "" },
        { NULL } /* sentinel */
};

static void result_callback(XIMU3_Result data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const tuple = Py_BuildValue("(i)", data);
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);

    PyGILState_Release(state);
}

#endif
