#ifndef FILE_CONVERTER_STATUS_H
#define FILE_CONVERTER_STATUS_H

#include "../../C/Ximu3.h"
#include <Python.h>

static PyObject *file_converter_status_to_string(PyObject *self, PyObject *args) {
    int status_int;

    if (PyArg_ParseTuple(args, "i", &status_int) == 0) {
        return NULL;
    }

    const XIMU3_FileConverterStatus status = (XIMU3_FileConverterStatus) status_int;

    switch (status) {
        case XIMU3_FileConverterStatusComplete:
        case XIMU3_FileConverterStatusFailed:
        case XIMU3_FileConverterStatusInProgress:
            break;
        default:
            PyErr_SetString(PyExc_ValueError, "Expected FILE_CONVERTER_STATUS_*");
            return NULL;
    }

    const char *const string = XIMU3_file_converter_status_to_string(status);

    return PyUnicode_FromString(string);
}

static PyMethodDef file_converter_status_methods[] = {
    {"file_converter_status_to_string", (PyCFunction) file_converter_status_to_string, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

#endif
