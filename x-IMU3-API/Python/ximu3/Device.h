#ifndef DEVICE_H
#define DEVICE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_Device wrapped;
} Device;

static void device_free(Device *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *device_str(Device *self) {
    const char *const string = XIMU3_device_to_string(self->wrapped);

    return PyUnicode_FromString(string);
}

static PyObject *device_get_device_name(Device *self) {
    return PyUnicode_FromString(self->wrapped.device_name);
}

static PyObject *device_get_serial_number(Device *self) {
    return PyUnicode_FromString(self->wrapped.serial_number);
}

static PyObject *device_get_connection_config(Device *self) {
    switch (self->wrapped.connection_type) {
        case XIMU3_ConnectionTypeUsb:
            return usb_connection_config_from(&self->wrapped.usb_connection_config);
        case XIMU3_ConnectionTypeSerial:
            return serial_connection_config_from(&self->wrapped.serial_connection_config);
        case XIMU3_ConnectionTypeBluetooth:
            return bluetooth_connection_config_from(&self->wrapped.bluetooth_connection_config);
        default:
            return NULL;
    }
}

static PyGetSetDef device_get_set[] = {
    {"device_name", (getter) device_get_device_name, NULL, "", NULL},
    {"serial_number", (getter) device_get_serial_number, NULL, "", NULL},
    {"connection_config", (getter) device_get_connection_config, NULL, "", NULL},
    {NULL} /* sentinel */
};

static PyTypeObject device_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.Device",
    .tp_basicsize = sizeof(Device),
    .tp_dealloc = (destructor) device_free,
    .tp_str = (reprfunc) device_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = device_get_set,
};

static PyObject *device_from(const XIMU3_Device *const device) {
    Device *const self = (Device *) device_object.tp_alloc(&device_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->wrapped = *device;
    return (PyObject *) self;
}

static PyObject *devices_to_list_and_free(const XIMU3_Devices devices) {
    PyObject *list = PyList_New(devices.length);

    if (list == NULL) {
        goto cleanup;
    }

    for (uint32_t index = 0; index < devices.length; index++) {
        PyObject *const item = device_from(&devices.array[index]);

        if (item == NULL) {
            Py_DECREF(list);
            list = NULL;
            goto cleanup;
        }

        PyList_SetItem(list, index, item);
    }

cleanup:
    XIMU3_devices_free(devices);

    return list;
}

static void devices_callback(XIMU3_Devices data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = devices_to_list_and_free(data);

    if (object == NULL) {
        PyErr_Print();
        goto cleanup;
    }

    tuple = PyTuple_Pack(1, object);

    if (tuple == NULL) {
        PyErr_Print();
        goto cleanup;
    }

    result = PyObject_CallObject((PyObject *) context, tuple);

    if (result == NULL) {
        PyErr_Print();
    }

cleanup:
    Py_XDECREF(object);
    Py_XDECREF(tuple);
    Py_XDECREF(result);

    PyGILState_Release(state);
}

#endif
