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

    PyErr_SetString(PyExc_ValueError, "'file_converter_status' must be ximu3.FILE_CONVERTER_STATUS_*");
    return -1;
}

static PyObject *file_converter_status_to_string(PyObject *null, PyObject *arg) {
    const int file_converter_status_int = (int) PyLong_AsLong(arg);

    if (PyErr_Occurred()) {
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
    {"file_converter_status_to_string", (PyCFunction) file_converter_status_to_string, METH_O, ""},
    {NULL} /* sentinel */
};

#endif
