// This file was generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

#ifndef EARTH_ACCELERATION_MESSAGE_H
#define EARTH_ACCELERATION_MESSAGE_H

#include "../../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_EarthAccelerationMessage message;
} EarthAccelerationMessage;

static void earth_acceleration_message_free(EarthAccelerationMessage* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* earth_acceleration_message_get_timestamp(EarthAccelerationMessage* self)
{
    return Py_BuildValue("K", self->message.timestamp);
}

static PyObject* earth_acceleration_message_get_quaternion_w(EarthAccelerationMessage* self)
{
    return Py_BuildValue("f", self->message.quaternion_w);
}

static PyObject* earth_acceleration_message_get_quaternion_x(EarthAccelerationMessage* self)
{
    return Py_BuildValue("f", self->message.quaternion_x);
}

static PyObject* earth_acceleration_message_get_quaternion_y(EarthAccelerationMessage* self)
{
    return Py_BuildValue("f", self->message.quaternion_y);
}

static PyObject* earth_acceleration_message_get_quaternion_z(EarthAccelerationMessage* self)
{
    return Py_BuildValue("f", self->message.quaternion_z);
}

static PyObject* earth_acceleration_message_get_acceleration_x(EarthAccelerationMessage* self)
{
    return Py_BuildValue("f", self->message.acceleration_x);
}

static PyObject* earth_acceleration_message_get_acceleration_y(EarthAccelerationMessage* self)
{
    return Py_BuildValue("f", self->message.acceleration_y);
}

static PyObject* earth_acceleration_message_get_acceleration_z(EarthAccelerationMessage* self)
{
    return Py_BuildValue("f", self->message.acceleration_z);
}

static PyObject* earth_acceleration_message_to_string(EarthAccelerationMessage* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_earth_acceleration_message_to_string(self->message));
}

static PyObject* earth_acceleration_message_to_euler_angles_message(EarthAccelerationMessage* self, PyObject* args);

static PyGetSetDef earth_acceleration_message_get_set[] = {
    { "timestamp", (getter) earth_acceleration_message_get_timestamp, NULL, "", NULL },
    { "quaternion_w", (getter) earth_acceleration_message_get_quaternion_w, NULL, "", NULL },
    { "quaternion_x", (getter) earth_acceleration_message_get_quaternion_x, NULL, "", NULL },
    { "quaternion_y", (getter) earth_acceleration_message_get_quaternion_y, NULL, "", NULL },
    { "quaternion_z", (getter) earth_acceleration_message_get_quaternion_z, NULL, "", NULL },
    { "acceleration_x", (getter) earth_acceleration_message_get_acceleration_x, NULL, "", NULL },
    { "acceleration_y", (getter) earth_acceleration_message_get_acceleration_y, NULL, "", NULL },
    { "acceleration_z", (getter) earth_acceleration_message_get_acceleration_z, NULL, "", NULL },
    { NULL } /* sentinel */
};

static PyMethodDef earth_acceleration_message_methods[] = {
    { "to_string", (PyCFunction) earth_acceleration_message_to_string, METH_NOARGS, "" },
    { "to_euler_angles_message", (PyCFunction) earth_acceleration_message_to_euler_angles_message, METH_NOARGS, "" },
    { NULL } /* sentinel */
};

static PyTypeObject earth_acceleration_message_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.EarthAccelerationMessage",
    .tp_basicsize = sizeof(EarthAccelerationMessage),
    .tp_dealloc = (destructor) earth_acceleration_message_free,
    .tp_getset = earth_acceleration_message_get_set,
    .tp_methods = earth_acceleration_message_methods,
};

static PyObject* earth_acceleration_message_from(const XIMU3_EarthAccelerationMessage* const message)
{
    EarthAccelerationMessage* const self = (EarthAccelerationMessage*) earth_acceleration_message_object.tp_alloc(&earth_acceleration_message_object, 0);
    self->message = *message;
    return (PyObject*) self;
}

static void earth_acceleration_message_callback(XIMU3_EarthAccelerationMessage data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const object = earth_acceleration_message_from(&data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

#endif
