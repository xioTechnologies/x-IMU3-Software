#ifndef CHARGING_STATUS_H
#define CHARGING_STATUS_H

#include "../../C/Ximu3.h"
#include <Python.h>

static PyObject *charging_status_from_float(PyObject *self, PyObject *arg) {
    const float charging_status_float = (float) PyFloat_AsDouble(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    const XIMU3_ChargingStatus charging_status = XIMU3_charging_status_from_float(charging_status_float);

    return PyLong_FromLong((long) charging_status);
}

static int charging_status_from(XIMU3_ChargingStatus *const charging_status, const int charging_status_int) {
    switch (charging_status_int) {
        case XIMU3_ChargingStatusNotConnected:
        case XIMU3_ChargingStatusCharging:
        case XIMU3_ChargingStatusChargingComplete:
        case XIMU3_ChargingStatusChargingOnHold:
            *charging_status = (XIMU3_ChargingStatus) charging_status_int;
            return 0;
    }

    PyErr_SetString(PyExc_ValueError, "'charging_status' must be ximu3.CHARGING_STATUS_*");
    return -1;
}

static PyObject *charging_status_to_string(PyObject *null, PyObject *arg) {
    const int charging_status_int = (int) PyLong_AsLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    XIMU3_ChargingStatus charging_status;

    if (charging_status_from(&charging_status, charging_status_int) != 0) {
        return NULL;
    }

    const char *const string = XIMU3_charging_status_to_string(charging_status);

    return PyUnicode_FromString(string);
}

static PyMethodDef charging_status_methods[] = {
    {"charging_status_from_float", (PyCFunction) charging_status_from_float, METH_O, ""},
    {"charging_status_to_string", (PyCFunction) charging_status_to_string, METH_O, ""},
    {NULL} /* sentinel */
};

#endif
