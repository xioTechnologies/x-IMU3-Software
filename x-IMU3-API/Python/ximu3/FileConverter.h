#ifndef FILE_CONVERTER_H
#define FILE_CONVERTER_H

#include "../../C/Ximu3.h"
#include "FileConverterProgress.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_FileConverter *file_converter;
} FileConverter;

static PyObject *file_converter_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *destination;
    const char *name;
    PyObject *files_sequence;
    PyObject *callable;

    if (PyArg_ParseTuple(args, "ssOO:set_callback", &destination, &name, &files_sequence, &callable) == 0) {
        return NULL;
    }

    if (PySequence_Check(files_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, "'files' must be a sequence");
        return NULL;
    }

    const uint32_t length = (uint32_t) PySequence_Size(files_sequence);

    if (length > CHAR_PTR_ARRAY_LENGTH) {
        PyErr_Format(PyExc_ValueError, "'files' has too many items. Cannot exceed %d.", CHAR_PTR_ARRAY_LENGTH);
        return NULL;
    }

    const char *files[CHAR_PTR_ARRAY_LENGTH];

    for (uint32_t index = 0; index < length; index++) {
        PyObject *file = PySequence_GetItem(files_sequence, index); // TODO: this will never be destroyed (memory leak)

        files[index] = (char *) PyUnicode_AsUTF8(file);

        if (files[index] == NULL) {
            return NULL;
        }
    }

    if (PyCallable_Check(callable) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    FileConverter *const self = (FileConverter *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    Py_INCREF(callable); // TODO: this will never be destroyed (memory leak)

    self->file_converter = XIMU3_file_converter_new(destination, name, files, length, file_converter_progress_callback, callable);
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
        return NULL;
    }

    if (PySequence_Check(files_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, "'files' must be a sequence");
        return NULL;
    }

    const uint32_t length = (uint32_t) PySequence_Size(files_sequence);

    if (length > CHAR_PTR_ARRAY_LENGTH) {
        PyErr_Format(PyExc_ValueError, "'files' has too many items. Cannot exceed %d.", CHAR_PTR_ARRAY_LENGTH);
        return NULL;
    }

    const char *files[CHAR_PTR_ARRAY_LENGTH];

    for (uint32_t index = 0; index < length; index++) {
        PyObject *file = PySequence_GetItem(files_sequence, index); // TODO: this will never be destroyed (memory leak)

        files[index] = (char *) PyUnicode_AsUTF8(file);

        if (files[index] == NULL) {
            return NULL;
        }
    }

    const XIMU3_FileConverterProgress progress = XIMU3_file_converter_convert(destination, name, files, length);

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
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = file_converter_new,
    .tp_methods = file_converter_methods,
};

#endif
