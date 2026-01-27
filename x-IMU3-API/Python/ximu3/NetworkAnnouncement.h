#ifndef NETWORK_ANNOUNCEMENT_H
#define NETWORK_ANNOUNCEMENT_H

#include "../../C/Ximu3.h"
#include "NetworkAnnouncementMessage.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_NetworkAnnouncement *wrapped;
} NetworkAnnouncement;

static PyObject *network_announcement_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    if (PyArg_ParseTuple(args, "") == 0) {
        return NULL;
    }

    XIMU3_NetworkAnnouncement *const network_announcement = XIMU3_network_announcement_new();

    const XIMU3_Result result = XIMU3_network_announcement_get_result(network_announcement);

    if (result != XIMU3_ResultOk) {
        const char *const result_string = XIMU3_result_to_string(result);

        PyErr_SetString(PyExc_RuntimeError, result_string);

        XIMU3_network_announcement_free(network_announcement);
        return NULL;
    }

    NetworkAnnouncement *const self = (NetworkAnnouncement *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        XIMU3_network_announcement_free(network_announcement);
        return NULL;
    }

    self->wrapped = network_announcement;
    return (PyObject *) self;
}

static void network_announcement_free(NetworkAnnouncement *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_network_announcement_free(self->wrapped);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *network_announcement_add_callback(NetworkAnnouncement *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_network_announcement_add_callback(self->wrapped, network_announcement_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *network_announcement_remove_callback(NetworkAnnouncement *self, PyObject *arg) {
    const unsigned long long callback_id = PyLong_AsUnsignedLongLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_network_announcement_remove_callback(self->wrapped, (uint64_t) callback_id);
    Py_END_ALLOW_THREADS

    Py_RETURN_NONE;
}

static PyObject *network_announcement_get_messages(NetworkAnnouncement *self, PyObject *args) {
    const XIMU3_NetworkAnnouncementMessages messages = XIMU3_network_announcement_get_messages(self->wrapped);

    return network_announcement_messages_to_list_and_free(messages);
}

static PyObject *network_announcement_get_messages_after_short_delay(NetworkAnnouncement *self, PyObject *args) {
    const XIMU3_NetworkAnnouncementMessages messages = XIMU3_network_announcement_get_messages_after_short_delay(self->wrapped);

    return network_announcement_messages_to_list_and_free(messages);
}

static PyMethodDef network_announcement_methods[] = {
    {"add_callback", (PyCFunction) network_announcement_add_callback, METH_O, ""},
    {"remove_callback", (PyCFunction) network_announcement_remove_callback, METH_O, ""},
    {"get_messages", (PyCFunction) network_announcement_get_messages, METH_NOARGS, ""},
    {"get_messages_after_short_delay", (PyCFunction) network_announcement_get_messages_after_short_delay, METH_NOARGS, ""},
    {NULL} /* sentinel */
};

static PyTypeObject network_announcement_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.NetworkAnnouncement",
    .tp_basicsize = sizeof(NetworkAnnouncement),
    .tp_dealloc = (destructor) network_announcement_free,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = network_announcement_new,
    .tp_methods = network_announcement_methods,
};

#endif
