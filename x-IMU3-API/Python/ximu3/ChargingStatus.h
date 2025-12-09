#ifndef CHARGING_STATUS_H
#define CHARGING_STATUS_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject* charging_status_from_float(PyObject* self, PyObject* args)
{
    float charging_status;

    if (PyArg_ParseTuple(args, "f", &charging_status) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    return Py_BuildValue("i", (int) XIMU3_charging_status_from_float(charging_status));
}

static PyObject* charging_status_to_string(PyObject* self, PyObject* args)
{
    int charging_status;

    if (PyArg_ParseTuple(args, "i", &charging_status) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    return Py_BuildValue("s", XIMU3_charging_status_to_string(XIMU3_charging_status_from_float((float) charging_status)));
}

static PyMethodDef charging_status_methods[] = {
    { "charging_status_from_float", (PyCFunction) charging_status_from_float, METH_VARARGS, "" },
    { "charging_status_to_string", (PyCFunction) charging_status_to_string, METH_VARARGS, "" },
    { NULL } /* sentinel */
};

#endif
