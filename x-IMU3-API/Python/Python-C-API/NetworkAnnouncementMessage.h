#ifndef NETWORK_ANNOUNCEMENT_MESSAGE_H
#define NETWORK_ANNOUNCEMENT_MESSAGE_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_NetworkAnnouncementMessage message;
} NetworkAnnouncementMessage;

static void network_announcement_message_free(NetworkAnnouncementMessage* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* network_announcement_message_get_device_name(NetworkAnnouncementMessage* self)
{
    return Py_BuildValue("s", self->message.device_name);
}

static PyObject* network_announcement_message_get_serial_number(NetworkAnnouncementMessage* self)
{
    return Py_BuildValue("s", self->message.serial_number);
}

static PyObject* network_announcement_message_get_rssi(NetworkAnnouncementMessage* self)
{
    return Py_BuildValue("i", self->message.rssi);
}

static PyObject* network_announcement_message_get_battery(NetworkAnnouncementMessage* self)
{
    return Py_BuildValue("i", self->message.battery);
}

static PyObject* network_announcement_message_get_status(NetworkAnnouncementMessage* self)
{
    return Py_BuildValue("s", XIMU3_charging_status_to_string(self->message.status));
}

static PyObject* network_announcement_message_get_tcp_connection_info(NetworkAnnouncementMessage* self)
{
    return tcp_connection_info_from(&self->message.tcp_connection_info);
}

static PyObject* network_announcement_message_get_udp_connection_info(NetworkAnnouncementMessage* self)
{
    return udp_connection_info_from(&self->message.udp_connection_info);
}

static PyObject* network_announcement_message_to_string(NetworkAnnouncementMessage* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_network_announcement_message_to_string(self->message));
}

static PyGetSetDef network_announcement_message_get_set[] = {
        { "device_name",         (getter) network_announcement_message_get_device_name,         NULL, "", NULL },
        { "serial_number",       (getter) network_announcement_message_get_serial_number,       NULL, "", NULL },
        { "rssi",                (getter) network_announcement_message_get_rssi,                NULL, "", NULL },
        { "battery",             (getter) network_announcement_message_get_battery,             NULL, "", NULL },
        { "status",              (getter) network_announcement_message_get_status,              NULL, "", NULL },
        { "tcp_connection_info", (getter) network_announcement_message_get_tcp_connection_info, NULL, "", NULL },
        { "udp_connection_info", (getter) network_announcement_message_get_udp_connection_info, NULL, "", NULL },
        { NULL }  /* sentinel */
};

static PyMethodDef network_announcement_message_methods[] = {
        { "to_string", (PyCFunction) network_announcement_message_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject network_announcement_message_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.NetworkAnnouncementMessage",
        .tp_basicsize = sizeof(NetworkAnnouncementMessage),
        .tp_dealloc = (destructor) network_announcement_message_free,
        .tp_new = PyType_GenericNew,
        .tp_getset = network_announcement_message_get_set,
        .tp_methods = network_announcement_message_methods,
};

static PyObject* network_announcement_message_from(const XIMU3_NetworkAnnouncementMessage* const message)
{
    NetworkAnnouncementMessage* const self = (NetworkAnnouncementMessage*) network_announcement_message_object.tp_alloc(&network_announcement_message_object, 0);
    self->message = *message;
    return (PyObject*) self;
}

static PyObject* network_announcement_messages_to_list_and_free(const XIMU3_NetworkAnnouncementMessages messages)
{
    PyObject* const message_list = PyList_New(messages.length);

    for (uint32_t index = 0; index < messages.length; index++)
    {
        PyList_SetItem(message_list, index, network_announcement_message_from(&messages.array[index]));
    }

    XIMU3_network_announcement_messages_free(messages);
    return message_list;
}

typedef struct
{
    PyObject* callable;
    XIMU3_NetworkAnnouncementMessage data;
} NetworkAnnouncementMessageCallbackPendingCallArg;

static int network_announcement_messages_pending_call_func(void* arg)
{
    PyObject* const object = network_announcement_message_from(&((NetworkAnnouncementMessageCallbackPendingCallArg*) arg)->data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject(((NetworkAnnouncementMessageCallbackPendingCallArg*) arg)->callable, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);
    free(arg);
    return 0;
}

static void network_announcement_message_callback(XIMU3_NetworkAnnouncementMessage data, void* context)
{
    NetworkAnnouncementMessageCallbackPendingCallArg* const arg = malloc(sizeof(NetworkAnnouncementMessageCallbackPendingCallArg));
    arg->callable = (PyObject*) context;
    arg->data = data;
    Py_AddPendingCall(&network_announcement_messages_pending_call_func, arg);
}

#endif
