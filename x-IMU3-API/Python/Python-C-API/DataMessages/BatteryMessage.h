// This file was generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#ifndef BATTERY_MESSAGE_H
#define BATTERY_MESSAGE_H

#include "../../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_BatteryMessage message;
} BatteryMessage;

static void battery_message_free(BatteryMessage* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* battery_message_get_timestamp(BatteryMessage* self)
{
    return Py_BuildValue("K", self->message.timestamp);
}

static PyObject* battery_message_get_percentage(BatteryMessage* self)
{
    return Py_BuildValue("f", self->message.percentage);
}

static PyObject* battery_message_get_voltage(BatteryMessage* self)
{
    return Py_BuildValue("f", self->message.voltage);
}

static PyObject* battery_message_get_charging_status(BatteryMessage* self)
{
    return Py_BuildValue("f", self->message.charging_status);
}

static PyObject* battery_message_to_string(BatteryMessage* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_battery_message_to_string(self->message));
}

static PyGetSetDef battery_message_get_set[] = {
        { "timestamp",       (getter) battery_message_get_timestamp,       NULL, "", NULL },
        { "percentage",      (getter) battery_message_get_percentage,      NULL, "", NULL },
        { "voltage",         (getter) battery_message_get_voltage,         NULL, "", NULL },
        { "charging_status", (getter) battery_message_get_charging_status, NULL, "", NULL },
        { NULL }  /* sentinel */
};

static PyMethodDef battery_message_methods[] = {
        { "to_string", (PyCFunction) battery_message_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject battery_message_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.BatteryMessage",
        .tp_basicsize = sizeof(BatteryMessage),
        .tp_dealloc = (destructor) battery_message_free,
        .tp_getset = battery_message_get_set,
        .tp_methods = battery_message_methods,
};

static PyObject* battery_message_from(const XIMU3_BatteryMessage* const message)
{
    BatteryMessage* const self = (BatteryMessage*) battery_message_object.tp_alloc(&battery_message_object, 0);
    self->message = *message;
    return (PyObject*) self;
}

static void battery_message_callback(XIMU3_BatteryMessage data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const object = battery_message_from(&data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

#endif
