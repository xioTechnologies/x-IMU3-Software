#ifndef DISCOVERED_SERIAL_DEVICE_H
#define DISCOVERED_SERIAL_DEVICE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_DiscoveredSerialDevice device;
} DiscoveredSerialDevice;

static void discovered_serial_device_free(DiscoveredSerialDevice* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* discovered_serial_device_get_device_name(DiscoveredSerialDevice* self)
{
    return Py_BuildValue("s", self->device.device_name);
}

static PyObject* discovered_serial_device_get_serial_number(DiscoveredSerialDevice* self)
{
    return Py_BuildValue("s", self->device.serial_number);
}

static PyObject* discovered_serial_device_get_connection_info(DiscoveredSerialDevice* self)
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

static PyObject* discovered_serial_device_to_string(DiscoveredSerialDevice* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_discovered_serial_device_to_string(self->device));
}

static PyGetSetDef discovered_serial_device_get_set[] = {
        { "device_name",     (getter) discovered_serial_device_get_device_name,     NULL, "", NULL },
        { "serial_number",   (getter) discovered_serial_device_get_serial_number,   NULL, "", NULL },
        { "connection_info", (getter) discovered_serial_device_get_connection_info, NULL, "", NULL },
        { NULL }  /* sentinel */
};

static PyMethodDef discovered_serial_device_methods[] = {
        { "to_string", (PyCFunction) discovered_serial_device_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject discovered_serial_device_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.DiscoveredSerialDevice",
        .tp_basicsize = sizeof(DiscoveredSerialDevice),
        .tp_dealloc = (destructor) discovered_serial_device_free,
        .tp_new = PyType_GenericNew,
        .tp_getset = discovered_serial_device_get_set,
        .tp_methods = discovered_serial_device_methods,
};

static PyObject* discovered_serial_device_from(const XIMU3_DiscoveredSerialDevice* const device)
{
    DiscoveredSerialDevice* const self = (DiscoveredSerialDevice*) discovered_serial_device_object.tp_alloc(&discovered_serial_device_object, 0);
    self->device = *device;
    return (PyObject*) self;
}

static PyObject* discovered_serial_devices_to_list_and_free(const XIMU3_DiscoveredSerialDevices devices)
{
    PyObject* const device_list = PyList_New(devices.length);

    for (uint32_t index = 0; index < devices.length; index++)
    {
        PyList_SetItem(device_list, index, discovered_serial_device_from(&devices.array[index]));
    }

    XIMU3_discovered_serial_devices_free(devices);
    return device_list;
}

typedef struct
{
    PyObject* callable;
    XIMU3_DiscoveredSerialDevices data;
} DiscoveredSerialDevicesCallbackPendingCallArg;

static int discovered_serial_devices_pending_call_func(void* arg)
{
    PyObject* const object = discovered_serial_devices_to_list_and_free(((DiscoveredSerialDevicesCallbackPendingCallArg*) arg)->data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject(((DiscoveredSerialDevicesCallbackPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);
    free(arg);
    return 0;
}

static void discovered_serial_devices_callback(XIMU3_DiscoveredSerialDevices data, void* context)
{
    DiscoveredSerialDevicesCallbackPendingCallArg* const arg = malloc(sizeof(DiscoveredSerialDevicesCallbackPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = data;
    Py_AddPendingCall(&discovered_serial_devices_pending_call_func, arg);
}

#endif
