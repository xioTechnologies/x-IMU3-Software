#ifndef SETTINGS_H
#define SETTINGS_H

#include "../../../C/Ximu3.h"
#include "Connection.h"
#include <Python.h>

static PyObject *settings_backup(PyObject *self, PyObject *args) {
    const char *file_path;
    PyObject *connection;

    if (PyArg_ParseTuple(args, "sO!", &file_path, &connection_object, &connection) == 0) {
        return NULL;
    }

    const XIMU3_Result result = XIMU3_settings_backup(file_path, ((Connection *) connection)->wrapped);

    if (result != XIMU3_ResultOk) {
        PyErr_Format(PyExc_RuntimeError, "Backup failed. %s.", XIMU3_result_to_string(result));
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *settings_restore(PyObject *self, PyObject *args) {
    const char *file_path;
    PyObject *connection;

    if (PyArg_ParseTuple(args, "sO!", &file_path, &connection_object, &connection) == 0) {
        return NULL;
    }

    const XIMU3_Result result = XIMU3_settings_restore(file_path, ((Connection *) connection)->wrapped);

    if (result != XIMU3_ResultOk) {
        PyErr_Format(PyExc_RuntimeError, "Restore failed. %s.", XIMU3_result_to_string(result));
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef settings_methods[] = {
    {"settings_backup", (PyCFunction) settings_backup, METH_VARARGS, ""},
    {"settings_restore", (PyCFunction) settings_restore, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

#endif
