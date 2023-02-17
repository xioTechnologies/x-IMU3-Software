#ifndef CHARGING_STATUS_H
#define CHARGING_STATUS_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject* charging_status_to_string(PyObject* self, PyObject* args)
{
    int charging_status_int;

    if (PyArg_ParseTuple(args, "i", &charging_status_int) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const XIMU3_ChargingStatus charging_status_enum = (XIMU3_ChargingStatus) charging_status_int;

    switch (charging_status_enum)
    {
        case XIMU3_ChargingStatusNotConnected:
        case XIMU3_ChargingStatusCharging:
        case XIMU3_ChargingStatusChargingComplete:
            return Py_BuildValue("s", XIMU3_charging_status_to_string(charging_status_enum));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef charging_status_methods[] = {
        { "charging_status_to_string", (PyCFunction) charging_status_to_string, METH_VARARGS, "" },
        { NULL } /* sentinel */
};

#endif
