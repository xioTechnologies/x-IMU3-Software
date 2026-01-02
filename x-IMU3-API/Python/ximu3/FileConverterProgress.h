#ifndef FILE_CONVERTER_PROGRESS_H
#define FILE_CONVERTER_PROGRESS_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_FileConverterProgress progress;
} FileConverterProgress;

static void file_converter_progress_free(FileConverterProgress *self) {
    Py_TYPE(self)->tp_free(self);
}

static PyObject *file_converter_progress_get_status(FileConverterProgress *self) {
    return PyLong_FromLong((long) self->progress.status);
}

static PyObject *file_converter_progress_get_percentage(FileConverterProgress *self) {
    return PyFloat_FromDouble((double) self->progress.percentage);
}

static PyObject *file_converter_progress_get_bytes_processed(FileConverterProgress *self) {
    return PyLong_FromUnsignedLongLong((unsigned long long) self->progress.bytes_processed);
}

static PyObject *file_converter_progress_get_bytes_total(FileConverterProgress *self) {
    return PyLong_FromUnsignedLongLong((unsigned long long) self->progress.bytes_total);
}

static PyObject *file_converter_progress_to_string(FileConverterProgress *self, PyObject *args) {
    const char *const string = XIMU3_file_converter_progress_to_string(self->progress);

    return PyUnicode_FromString(string);
}

static PyGetSetDef file_converter_progress_get_set[] = {
    {"status", (getter) file_converter_progress_get_status, NULL, "", NULL},
    {"percentage", (getter) file_converter_progress_get_percentage, NULL, "", NULL},
    {"bytes_processed", (getter) file_converter_progress_get_bytes_processed, NULL, "", NULL},
    {"bytes_total", (getter) file_converter_progress_get_bytes_total, NULL, "", NULL},
    {NULL} /* sentinel */
};

static PyMethodDef file_converter_progress_methods[] = {
    {"to_string", (PyCFunction) file_converter_progress_to_string, METH_NOARGS, ""},
    {NULL} /* sentinel */
};

static PyTypeObject file_converter_progress_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.FileConverterProgress",
    .tp_basicsize = sizeof(FileConverterProgress),
    .tp_dealloc = (destructor) file_converter_progress_free,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_getset = file_converter_progress_get_set,
    .tp_methods = file_converter_progress_methods,
};

static PyObject *file_converter_progress_from(const XIMU3_FileConverterProgress *const progress) {
    FileConverterProgress *const self = (FileConverterProgress *) file_converter_progress_object.tp_alloc(&file_converter_progress_object, 0);

    if (self == NULL) {
        return NULL;
    }

    self->progress = *progress;
    return (PyObject *) self;
}

static void file_converter_progress_callback(XIMU3_FileConverterProgress data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = file_converter_progress_from(&data);

    if (object == NULL) {
        PyErr_Print();
        goto cleanup;
    }

    tuple = PyTuple_Pack(1, object);

    if (tuple == NULL) {
        PyErr_Print();
        goto cleanup;
    }

    result = PyObject_CallObject((PyObject *) context, tuple);

    if (result == NULL) {
        PyErr_Print();
    }

cleanup:
    Py_XDECREF(object);
    Py_XDECREF(tuple);
    Py_XDECREF(result);

    PyGILState_Release(state);
}

#endif
