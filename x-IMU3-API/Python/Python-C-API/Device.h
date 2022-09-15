#ifndef DEVICE_H
#define DEVICE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_Device device;
} Device;

static void device_free(Device* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* device_get_device_name(Device* self)
{
    return Py_BuildValue("s", self->device.device_name);
}

static PyObject* device_get_serial_number(Device* self)
{
    return Py_BuildValue("s", self->device.serial_number);
}

static PyObject* device_get_connection_info(Device* self)
{
    switch (self->device.connection_type)
    {
        case XIMU3_ConnectionTypeUsb:
            return usb_connection_info_from(&self->device.usb_connection_info);
        case XIMU3_ConnectionTypeSerial:
            return serial_connection_info_from(&self->device.serial_connection_info);
        case XIMU3_ConnectionTypeBluetooth:
            return bluetooth_connection_info_from(&self->device.bluetooth_connection_info);
        default:
            return NULL;
    }
}

static PyObject* device_to_string(Device* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_device_to_string(self->device));
}

static PyGetSetDef device_get_set[] = {
        { "device_name",     (getter) device_get_device_name,     NULL, "", NULL },
        { "serial_number",   (getter) device_get_serial_number,   NULL, "", NULL },
        { "connection_info", (getter) device_get_connection_info, NULL, "", NULL },
        { NULL }  /* sentinel */
};

static PyMethodDef device_methods[] = {
        { "to_string", (PyCFunction) device_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject device_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.Device",
        .tp_basicsize = sizeof(Device),
        .tp_dealloc = (destructor) device_free,
        .tp_new = PyType_GenericNew,
        .tp_getset = device_get_set,
        .tp_methods = device_methods,
};

static PyObject* device_from(const XIMU3_Device* const device)
{
    Device* const self = (Device*) device_object.tp_alloc(&device_object, 0);
    self->device = *device;
    return (PyObject*) self;
}

static PyObject* devices_to_list_and_free(const XIMU3_Devices devices)
{
    PyObject* const device_list = PyList_New(devices.length);

    for (uint32_t index = 0; index < devices.length; index++)
    {
        PyList_SetItem(device_list, index, device_from(&devices.array[index]));
    }

    XIMU3_devices_free(devices);
    return device_list;
}

typedef struct
{
    PyObject* callable;
    XIMU3_Devices data;
} DevicesCallbackPendingCallArg;

static int devices_pending_call_func(void* arg)
{
    PyObject* const object = devices_to_list_and_free(((DevicesCallbackPendingCallArg*) arg)->data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject(((DevicesCallbackPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);
    free(arg);
    return 0;
}

static void devices_callback(XIMU3_Devices data, void* context)
{
    DevicesCallbackPendingCallArg* const arg = malloc(sizeof(DevicesCallbackPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = data;
    Py_AddPendingCall(&devices_pending_call_func, arg);
}

#endif
