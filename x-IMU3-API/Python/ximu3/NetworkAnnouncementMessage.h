#ifndef NETWORK_ANNOUNCEMENT_MESSAGE_H
#define NETWORK_ANNOUNCEMENT_MESSAGE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_NetworkAnnouncementMessage message;
} NetworkAnnouncementMessage;

static void network_announcement_message_free(NetworkAnnouncementMessage *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *network_announcement_message_str(NetworkAnnouncementMessage *self) {
    const char *const string = XIMU3_network_announcement_message_to_string(self->message);

    return PyUnicode_FromString(string);
}

static PyObject *network_announcement_message_get_device_name(NetworkAnnouncementMessage *self) {
    return PyUnicode_FromString(self->message.device_name);
}

static PyObject *network_announcement_message_get_serial_number(NetworkAnnouncementMessage *self) {
    return PyUnicode_FromString(self->message.serial_number);
}

static PyObject *network_announcement_message_get_ip_address(NetworkAnnouncementMessage *self) {
    return PyUnicode_FromString(self->message.ip_address);
}

static PyObject *network_announcement_message_get_tcp_port(NetworkAnnouncementMessage *self) {
    return PyLong_FromUnsignedLong((unsigned long) self->message.tcp_port);
}

static PyObject *network_announcement_message_get_udp_send(NetworkAnnouncementMessage *self) {
    return PyLong_FromUnsignedLong((unsigned long) self->message.udp_send);
}

static PyObject *network_announcement_message_get_udp_receive(NetworkAnnouncementMessage *self) {
    return PyLong_FromUnsignedLong((unsigned long) self->message.udp_receive);
}

static PyObject *network_announcement_message_get_rssi(NetworkAnnouncementMessage *self) {
    return PyLong_FromLong((long) self->message.rssi);
}

static PyObject *network_announcement_message_get_battery(NetworkAnnouncementMessage *self) {
    return PyLong_FromLong((long) self->message.battery);
}

static PyObject *network_announcement_message_get_charging_status(NetworkAnnouncementMessage *self) {
    return PyLong_FromLong((long) self->message.charging_status);
}

static PyObject *network_announcement_message_to_tcp_connection_info(NetworkAnnouncementMessage *self) {
    const XIMU3_TcpConnectionInfo connection_info = XIMU3_network_announcement_message_to_tcp_connection_info(self->message);

    return tcp_connection_info_from(&connection_info);
}

static PyObject *network_announcement_message_to_udp_connection_info(NetworkAnnouncementMessage *self) {
    const XIMU3_UdpConnectionInfo connection_info = XIMU3_network_announcement_message_to_udp_connection_info(self->message);

    return udp_connection_info_from(&connection_info);
}

static PyGetSetDef network_announcement_message_get_set[] = {
    {"device_name", (getter) network_announcement_message_get_device_name, NULL, "", NULL},
    {"serial_number", (getter) network_announcement_message_get_serial_number, NULL, "", NULL},
    {"ip_address", (getter) network_announcement_message_get_ip_address, NULL, "", NULL},
    {"tcp_port", (getter) network_announcement_message_get_tcp_port, NULL, "", NULL},
    {"udp_send", (getter) network_announcement_message_get_udp_send, NULL, "", NULL},
    {"udp_receive", (getter) network_announcement_message_get_udp_receive, NULL, "", NULL},
    {"rssi", (getter) network_announcement_message_get_rssi, NULL, "", NULL},
    {"battery", (getter) network_announcement_message_get_battery, NULL, "", NULL},
    {"charging_status", (getter) network_announcement_message_get_charging_status, NULL, "", NULL},
    {NULL} /* sentinel */
};

static PyMethodDef network_announcement_message_methods[] = {
    {"to_tcp_connection_info", (PyCFunction) network_announcement_message_to_tcp_connection_info, METH_NOARGS, ""},
    {"to_udp_connection_info", (PyCFunction) network_announcement_message_to_udp_connection_info, METH_NOARGS, ""},
    {NULL} /* sentinel */
};

static PyTypeObject network_announcement_message_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.NetworkAnnouncementMessage",
    .tp_basicsize = sizeof(NetworkAnnouncementMessage),
    .tp_dealloc = (destructor) network_announcement_message_free,
    .tp_str = (reprfunc) network_announcement_message_str,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = network_announcement_message_get_set,
    .tp_methods = network_announcement_message_methods,
};

static PyObject *network_announcement_message_from(const XIMU3_NetworkAnnouncementMessage *const message) {
    NetworkAnnouncementMessage *const self = (NetworkAnnouncementMessage *) network_announcement_message_object.tp_alloc(&network_announcement_message_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->message = *message;
    return (PyObject *) self;
}

static PyObject *network_announcement_messages_to_list_and_free(const XIMU3_NetworkAnnouncementMessages messages) {
    PyObject *list = PyList_New(messages.length);

    if (list == NULL) {
        goto cleanup;
    }

    for (uint32_t index = 0; index < messages.length; index++) {
        PyObject *const item = network_announcement_message_from(&messages.array[index]);

        if (item == NULL) {
            Py_DECREF(list);
            list = NULL;
            goto cleanup;
        }

        PyList_SetItem(list, index, item);
    }

cleanup:
    XIMU3_network_announcement_messages_free(messages);

    return list;
}

static void network_announcement_message_callback(XIMU3_NetworkAnnouncementMessage data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = network_announcement_message_from(&data);

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
