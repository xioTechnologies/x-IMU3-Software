// This file was generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#ifndef AHRS_STATUS_MESSAGE_H
#define AHRS_STATUS_MESSAGE_H

#include "../../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_AhrsStatusMessage message;
} AhrsStatusMessage;

static void ahrs_status_message_free(AhrsStatusMessage* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* ahrs_status_message_get_timestamp(AhrsStatusMessage* self)
{
    return Py_BuildValue("K", self->message.timestamp);
}

static PyObject* ahrs_status_message_get_initialising(AhrsStatusMessage* self)
{
    return Py_BuildValue("f", self->message.initialising);
}

static PyObject* ahrs_status_message_get_angular_rate_recovery(AhrsStatusMessage* self)
{
    return Py_BuildValue("f", self->message.angular_rate_recovery);
}

static PyObject* ahrs_status_message_get_acceleration_recovery(AhrsStatusMessage* self)
{
    return Py_BuildValue("f", self->message.acceleration_recovery);
}

static PyObject* ahrs_status_message_get_magnetic_recovery(AhrsStatusMessage* self)
{
    return Py_BuildValue("f", self->message.magnetic_recovery);
}

static PyObject* ahrs_status_message_to_string(AhrsStatusMessage* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_ahrs_status_message_to_string(self->message));
}

static PyGetSetDef ahrs_status_message_get_set[] = {
        { "timestamp",             (getter) ahrs_status_message_get_timestamp,             NULL, "", NULL },
        { "initialising",          (getter) ahrs_status_message_get_initialising,          NULL, "", NULL },
        { "angular_rate_recovery", (getter) ahrs_status_message_get_angular_rate_recovery, NULL, "", NULL },
        { "acceleration_recovery", (getter) ahrs_status_message_get_acceleration_recovery, NULL, "", NULL },
        { "magnetic_recovery",     (getter) ahrs_status_message_get_magnetic_recovery,     NULL, "", NULL },
        { NULL } /* sentinel */
};

static PyMethodDef ahrs_status_message_methods[] = {
        { "to_string", (PyCFunction) ahrs_status_message_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject ahrs_status_message_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.AhrsStatusMessage",
        .tp_basicsize = sizeof(AhrsStatusMessage),
        .tp_dealloc = (destructor) ahrs_status_message_free,
        .tp_getset = ahrs_status_message_get_set,
        .tp_methods = ahrs_status_message_methods,
};

static PyObject* ahrs_status_message_from(const XIMU3_AhrsStatusMessage* const message)
{
    AhrsStatusMessage* const self = (AhrsStatusMessage*) ahrs_status_message_object.tp_alloc(&ahrs_status_message_object, 0);
    self->message = *message;
    return (PyObject*) self;
}

static void ahrs_status_message_callback(XIMU3_AhrsStatusMessage data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const object = ahrs_status_message_from(&data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

#endif
