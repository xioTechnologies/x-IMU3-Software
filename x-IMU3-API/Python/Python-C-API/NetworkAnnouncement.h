#ifndef NETWORK_ANNOUNCEMENT_H
#define NETWORK_ANNOUNCEMENT_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include "NetworkAnnouncementMessage.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_NetworkAnnouncement* network_announcement;
} NetworkAnnouncement;

static PyObject* network_announcement_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
{
    NetworkAnnouncement* const self = (NetworkAnnouncement*) subtype->tp_alloc(subtype, 0);
    self->network_announcement = XIMU3_network_announcement_new();
    return (PyObject*) self;
}

static void network_announcement_free(NetworkAnnouncement* self)
{
    XIMU3_network_announcement_free(self->network_announcement);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* network_announcement_add_callback(NetworkAnnouncement* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_network_announcement_add_callback(self->network_announcement, network_announcement_message_callback, callable));
}

static PyObject* network_announcement_remove_callback(NetworkAnnouncement* self, PyObject* args)
{
    unsigned long long callback_id;
    if (PyArg_ParseTuple(args, "K", &callback_id))
    {
        XIMU3_network_announcement_remove_callback(self->network_announcement, (uint64_t) callback_id);

        Py_INCREF(Py_None);
        return Py_None;
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyObject* network_announcement_get_messages(NetworkAnnouncement* self, PyObject* args)
{
    return network_announcement_messages_to_list_and_free(XIMU3_network_announcement_get_messages(self->network_announcement));
}

static PyObject* network_announcement_get_messages_after_short_delay(NetworkAnnouncement* self, PyObject* args)
{
    return network_announcement_messages_to_list_and_free(XIMU3_network_announcement_get_messages_after_short_delay(self->network_announcement));
}

static PyMethodDef network_announcement_methods[] = {
        { "add_callback",                   (PyCFunction) network_announcement_add_callback,                   METH_VARARGS, "" },
        { "remove_callback",                (PyCFunction) network_announcement_remove_callback,                METH_VARARGS, "" },
        { "get_messages",                   (PyCFunction) network_announcement_get_messages,                   METH_NOARGS,  "" },
        { "get_messages_after_short_delay", (PyCFunction) network_announcement_get_messages_after_short_delay, METH_NOARGS,  "" },
        { NULL } /* sentinel */
};

static PyTypeObject network_announcement_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.NetworkAnnouncement",
        .tp_basicsize = sizeof(NetworkAnnouncement),
        .tp_dealloc = (destructor) network_announcement_free,
        .tp_new = network_announcement_new,
        .tp_methods = network_announcement_methods,
};

#endif
