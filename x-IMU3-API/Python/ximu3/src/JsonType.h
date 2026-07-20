#ifndef JSON_TYPE_H
#define JSON_TYPE_H

#include "../../../C/Ximu3.h"
#include <Python.h>

static int json_type_from(XIMU3_JsonType *const json_type, const int json_type_int) {
    switch (json_type_int) {
        case XIMU3_JsonTypeString:
        case XIMU3_JsonTypeNumber:
        case XIMU3_JsonTypeBoolean:
        case XIMU3_JsonTypeNull:
        case XIMU3_JsonTypeObject:
        case XIMU3_JsonTypeArray:
            *json_type = (XIMU3_JsonType) json_type_int;
            return 0;
    }

    PyErr_SetString(PyExc_ValueError, "'json_type' must be ximu3.JSON_TYPE_*");
    return -1;
}

static PyObject *json_type_to_string(PyObject *null, PyObject *arg) {
    const int json_type_int = (int) PyLong_AsLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    XIMU3_JsonType json_type;

    if (json_type_from(&json_type, json_type_int) != 0) {
        return NULL;
    }

    const char *const string = XIMU3_json_type_to_string(json_type);

    return PyUnicode_FromString(string);
}

static PyMethodDef json_type_methods[] = {
    {"json_type_to_string", (PyCFunction) json_type_to_string, METH_O, ""},
    {NULL} /* sentinel */
};

#endif
