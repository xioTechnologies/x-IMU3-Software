#ifndef FILE_CONVERTER_H
#define FILE_CONVERTER_H

#include "../../C/Ximu3.h"
#include "FileConverterProgress.h"
#include "Helpers.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_FileConverter *file_converter;
} FileConverter;

static PyObject *file_converter_new(PyTypeObject *subtype, PyObject *args, PyObject *keywords) {
    const char *destination;
    const char *name;
    PyObject *files_sequence;
    PyObject *callable;

    if (PyArg_ParseTuple(args, "ssOO:set_callback", &destination, &name, &files_sequence, &callable) == 0) {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PySequence_Check(files_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const char *files_char_ptr_array[CHAR_PTR_ARRAY_LENGTH];
    const uint32_t length = (uint32_t) PySequence_Size(files_sequence);

    for (uint32_t index = 0; index < length; index++) {
        if (index >= CHAR_PTR_ARRAY_LENGTH) {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        PyObject *file = PySequence_GetItem(files_sequence, index);

        if (PyUnicode_Check(file) == 0) {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        files_char_ptr_array[index] = (char *) PyUnicode_AsUTF8(file);
    }

    if (PyCallable_Check(callable) == 0) {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    FileConverter *const self = (FileConverter *) subtype->tp_alloc(subtype, 0);
    self->file_converter = XIMU3_file_converter_new(destination, name, files_char_ptr_array, length, file_converter_progress_callback, callable);
    return (PyObject *) self;
}

static void file_converter_free(FileConverter *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_file_converter_free(self->file_converter);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *file_converter_convert(PyObject *null, PyObject *args) {
    const char *destination;
    const char *name;
    PyObject *files_sequence;

    if (PyArg_ParseTuple(args, "ssO", &destination, &name, &files_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PySequence_Check(files_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const char *files_char_ptr_array[CHAR_PTR_ARRAY_LENGTH];
    const uint32_t length = (uint32_t) PySequence_Size(files_sequence);

    for (uint32_t index = 0; index < length; index++) {
        if (index >= CHAR_PTR_ARRAY_LENGTH) {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        PyObject *file = PySequence_GetItem(files_sequence, index);

        if (PyUnicode_Check(file) == 0) {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        files_char_ptr_array[index] = (char *) PyUnicode_AsUTF8(file);
    }

    const XIMU3_FileConverterProgress progress = XIMU3_file_converter_convert(destination, name, files_char_ptr_array, length);
    return file_converter_progress_from(&progress);
}

static PyMethodDef file_converter_methods[] = {
    {"convert", (PyCFunction) file_converter_convert, METH_VARARGS | METH_STATIC, ""},
    {NULL} /* sentinel */
};

static PyTypeObject file_converter_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.FileConverter",
    .tp_basicsize = sizeof(FileConverter),
    .tp_dealloc = (destructor) file_converter_free,
    .tp_new = file_converter_new,
    .tp_methods = file_converter_methods,
};

#endif
