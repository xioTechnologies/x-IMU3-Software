#ifndef CHARGING_STATUS_H
#define CHARGING_STATUS_H

#include "../../C/Ximu3.h"
#include <Python.h>

static PyObject *charging_status_from_float(PyObject *self, PyObject *args) {
    float charging_status_float;

    if (PyArg_ParseTuple(args, "f", &charging_status_float) == 0) {
        return NULL;
    }

    const XIMU3_ChargingStatus charging_status = XIMU3_charging_status_from_float(charging_status_float);

    return PyLong_FromLong((long) charging_status);
}

static PyObject *charging_status_to_string(PyObject *self, PyObject *args) {
    int charging_status_int;

    if (PyArg_ParseTuple(args, "i", &charging_status_int) == 0) {
        return NULL;
    }

    const XIMU3_ChargingStatus charging_status = (XIMU3_ChargingStatus) charging_status_int;

    switch (charging_status) {
        case XIMU3_ChargingStatusNotConnected:
        case XIMU3_ChargingStatusCharging:
        case XIMU3_ChargingStatusChargingComplete:
        case XIMU3_ChargingStatusChargingOnHold:
            break;
        default:
            PyErr_SetString(PyExc_ValueError, "Expected CHARGING_STATUS_*");
            return NULL;
    }

    const char *const string = XIMU3_charging_status_to_string(charging_status);

    return PyUnicode_FromString(string);
}

static PyMethodDef charging_status_methods[] = {
    {"charging_status_from_float", (PyCFunction) charging_status_from_float, METH_VARARGS, ""},
    {"charging_status_to_string", (PyCFunction) charging_status_to_string, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

#endif
