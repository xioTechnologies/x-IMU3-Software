#ifndef HELPERS_H
#define HELPERS_H

#include "../../C/Ximu3.h"
#include <Python.h>

#define INVALID_ARGUMENTS_STRING "Invalid arguments"

static PyObject* char_arrays_to_list_and_free(const XIMU3_CharArrays char_arrays)
{
    PyObject* const py_object = PyList_New(char_arrays.length);

    for (uint32_t index = 0; index < char_arrays.length; index++)
    {
        PyObject* const string = PyUnicode_FromString(char_arrays.array[index]);
        PyList_SetItem(py_object, index, string);
    }

    XIMU3_char_arrays_free(char_arrays);
    return py_object;
}

#endif
