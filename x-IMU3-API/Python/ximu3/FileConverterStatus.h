#ifndef FILE_CONVERTER_STATUS_H
#define FILE_CONVERTER_STATUS_H

#include "../../C/Ximu3.h"
#include <Python.h>

static int file_converter_status_from(XIMU3_FileConverterStatus *const file_converter_status, const int file_converter_status_int) {
    switch (file_converter_status_int) {
        case XIMU3_FileConverterStatusComplete:
        case XIMU3_FileConverterStatusFailed:
        case XIMU3_FileConverterStatusInProgress:
            *file_converter_status = (XIMU3_FileConverterStatus) file_converter_status_int;
            return 0;
    }

    PyErr_SetString(PyExc_ValueError, "'file_converter_status' must be FILE_CONVERTER_STATUS_*");
    return -1;
}

static PyObject *file_converter_status_to_string(PyObject *self, PyObject *args) {
    int file_converter_status_int;

    if (PyArg_ParseTuple(args, "i", &file_converter_status_int) == 0) {
        return NULL;
    }

    XIMU3_FileConverterStatus file_converter_status;

    if (file_converter_status_from(&file_converter_status, file_converter_status_int) != 0) {
        return NULL;
    }

    const char *const string = XIMU3_file_converter_status_to_string(file_converter_status);

    return PyUnicode_FromString(string);
}

static PyMethodDef file_converter_status_methods[] = {
    {"file_converter_status_to_string", (PyCFunction) file_converter_status_to_string, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

#endif
