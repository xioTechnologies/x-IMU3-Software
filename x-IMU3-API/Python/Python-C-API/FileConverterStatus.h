#ifndef FILE_CONVERTER_STATUS_H
#define FILE_CONVERTER_STATUS_H

#include "../../C/Ximu3.h"
#include "Helpers.h"
#include <Python.h>

static PyObject* file_converter_status_to_string(PyObject* self, PyObject* args)
{
    int file_converter_status_int;

    if (PyArg_ParseTuple(args, "i", &file_converter_status_int) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const XIMU3_FileConverterStatus file_converter_status_enum = (XIMU3_FileConverterStatus) file_converter_status_int;

    switch (file_converter_status_enum)
    {
        case XIMU3_FileConverterStatusComplete:
        case XIMU3_FileConverterStatusFailed:
        case XIMU3_FileConverterStatusInProgress:
            return Py_BuildValue("s", XIMU3_file_converter_status_to_string(file_converter_status_enum));
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef file_converter_status_methods[] = {
        { "file_converter_status_to_string", (PyCFunction) file_converter_status_to_string, METH_VARARGS, "" },
        { NULL } /* sentinel */
};

#endif
