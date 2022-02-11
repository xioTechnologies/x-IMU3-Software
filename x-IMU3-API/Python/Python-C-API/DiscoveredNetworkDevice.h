#ifndef DISCOVERED_NETWORK_DEVICE_H
#define DISCOVERED_NETWORK_DEVICE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_DiscoveredNetworkDevice device;
} DiscoveredNetworkDevice;

static void discovered_network_device_free(DiscoveredNetworkDevice* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* discovered_network_device_get_device_name(DiscoveredNetworkDevice* self)
{
    return Py_BuildValue("s", self->device.device_name);
}

static PyObject* discovered_network_device_get_serial_number(DiscoveredNetworkDevice* self)
{
    return Py_BuildValue("s", self->device.serial_number);
}

static PyObject* discovered_network_device_get_rssi(DiscoveredNetworkDevice* self)
{
    return Py_BuildValue("k", self->device.rssi);
}

static PyObject* discovered_network_device_get_battery(DiscoveredNetworkDevice* self)
{
    return Py_BuildValue("k", self->device.battery);
}

static PyObject* discovered_network_device_get_status(DiscoveredNetworkDevice* self)
{
    return Py_BuildValue("s", XIMU3_charging_status_to_string(self->device.status));
}

static PyObject* discovered_network_device_get_tcp_connection_info(DiscoveredNetworkDevice* self)
{
    return tcp_connection_info_from(&self->device.tcp_connection_info);
}

static PyObject* discovered_network_device_get_udp_connection_info(DiscoveredNetworkDevice* self)
{
    return udp_connection_info_from(&self->device.udp_connection_info);
}

static PyObject* discovered_network_device_to_string(DiscoveredNetworkDevice* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_discovered_network_device_to_string(self->device));
}

static PyGetSetDef discovered_network_device_get_set[] = {
        { "device_name",         (getter) discovered_network_device_get_device_name,         NULL, "", NULL },
        { "serial_number",       (getter) discovered_network_device_get_serial_number,       NULL, "", NULL },
        { "rssi",                (getter) discovered_network_device_get_rssi,                NULL, "", NULL },
        { "battery",             (getter) discovered_network_device_get_battery,             NULL, "", NULL },
        { "status",              (getter) discovered_network_device_get_status,              NULL, "", NULL },
        { "tcp_connection_info", (getter) discovered_network_device_get_tcp_connection_info, NULL, "", NULL },
        { "udp_connection_info", (getter) discovered_network_device_get_udp_connection_info, NULL, "", NULL },
        { NULL }  /* sentinel */
};

static PyMethodDef discovered_network_device_methods[] = {
        { "to_string", (PyCFunction) discovered_network_device_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject discovered_network_device_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.DiscoveredNetworkDevice",
        .tp_basicsize = sizeof(DiscoveredNetworkDevice),
        .tp_dealloc = (destructor) discovered_network_device_free,
        .tp_new = PyType_GenericNew,
        .tp_getset = discovered_network_device_get_set,
        .tp_methods = discovered_network_device_methods,
};

static PyObject* discovered_network_device_from(const XIMU3_DiscoveredNetworkDevice* const device)
{
    DiscoveredNetworkDevice* const self = (DiscoveredNetworkDevice*) discovered_network_device_object.tp_alloc(&discovered_network_device_object, 0);
    self->device = *device;
    return (PyObject*) self;
}

static PyObject* discovered_network_devices_to_list_and_free(const XIMU3_DiscoveredNetworkDevices devices)
{
    PyObject* const device_list = PyList_New(devices.length);

    for (uint32_t index = 0; index < devices.length; index++)
    {
        PyList_SetItem(device_list, index, discovered_network_device_from(&devices.array[index]));
    }

    XIMU3_discovered_network_devices_free(devices);
    return device_list;
}

typedef struct
{
    PyObject* callable;
    XIMU3_DiscoveredNetworkDevices data;
} DiscoveredNetworkDevicesCallbackPendingCallArg;

static int discovered_network_devices_pending_call_func(void* arg)
{
    PyObject* const object = discovered_network_devices_to_list_and_free(((DiscoveredNetworkDevicesCallbackPendingCallArg*) arg)->data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject(((DiscoveredNetworkDevicesCallbackPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);
    free(arg);
    return 0;
}

static void discovered_network_devices_callback(XIMU3_DiscoveredNetworkDevices data, void* context)
{
    DiscoveredNetworkDevicesCallbackPendingCallArg* const arg = malloc(sizeof(DiscoveredNetworkDevicesCallbackPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = data;
    Py_AddPendingCall(&discovered_network_devices_pending_call_func, arg);
}

#endif
