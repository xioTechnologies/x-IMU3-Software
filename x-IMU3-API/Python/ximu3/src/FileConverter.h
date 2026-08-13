#ifndef FILE_CONVERTER_H
#define FILE_CONVERTER_H

#include "../../../C/Ximu3.h"
#include "FileConverterProgress.h"
#include <Python.h>

typedef struct {
    PyObject_HEAD
    XIMU3_FileConverter *wrapped;
} FileConverter;

static PyObject *file_converter_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    const char *destination;
    const char *name;
    PyObject *file_paths_sequence;
    PyObject *callback;

    static char *kwlist[] = {
        "destination",
        "name",
        "file_paths",
        "callback",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "ssOO", kwlist, &destination, &name, &file_paths_sequence, &callback) == 0) {
        return NULL;
    }

    if (PySequence_Check(file_paths_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, "'file_paths' must be a sequence");
        return NULL;
    }

    const uint32_t length = (uint32_t) PySequence_Size(file_paths_sequence);

    if (length > CHAR_PTR_ARRAY_LENGTH) {
        PyErr_Format(PyExc_ValueError, "'file_paths' length must not exceed %d", CHAR_PTR_ARRAY_LENGTH);
        return NULL;
    }

    const char *file_paths[CHAR_PTR_ARRAY_LENGTH];

    for (uint32_t index = 0; index < length; index++) {
        PyObject *file_path = PySequence_GetItem(file_paths_sequence, index); // TODO: this will never be destroyed (memory leak)

        file_paths[index] = (char *) PyUnicode_AsUTF8(file_path);

        if (file_paths[index] == NULL) {
            return NULL;
        }
    }

    if (PyCallable_Check(callback) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    FileConverter *const self = (FileConverter *) subtype->tp_alloc(subtype, 0);

    if (self == NULL) {
        return NULL;
    }

    Py_INCREF(callback); // TODO: this will never be destroyed (memory leak)

    self->wrapped = XIMU3_file_converter_new(destination, name, file_paths, length, file_converter_progress_callback, callback);
    return (PyObject *) self;
}

static void file_converter_free(FileConverter *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_file_converter_free(self->wrapped);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *file_converter_convert(PyObject *null, PyObject *args, PyObject *kwds) {
    const char *destination;
    const char *name;
    PyObject *file_paths_sequence;

    static char *kwlist[] = {
        "destination",
        "name",
        "file_paths",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "ssO", kwlist, &destination, &name, &file_paths_sequence) == 0) {
        return NULL;
    }

    if (PySequence_Check(file_paths_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, "'file_paths' must be a sequence");
        return NULL;
    }

    const uint32_t length = (uint32_t) PySequence_Size(file_paths_sequence);

    if (length > CHAR_PTR_ARRAY_LENGTH) {
        PyErr_Format(PyExc_ValueError, "'file_paths' length must not exceed %d", CHAR_PTR_ARRAY_LENGTH);
        return NULL;
    }

    const char *file_paths[CHAR_PTR_ARRAY_LENGTH];

    for (uint32_t index = 0; index < length; index++) {
        PyObject *file_path = PySequence_GetItem(file_paths_sequence, index); // TODO: this will never be destroyed (memory leak)

        file_paths[index] = (char *) PyUnicode_AsUTF8(file_path);

        if (file_paths[index] == NULL) {
            return NULL;
        }
    }

    const XIMU3_FileConverterProgress progress = XIMU3_file_converter_convert(destination, name, file_paths, length);

    return file_converter_progress_from(&progress);
}

static PyMethodDef file_converter_methods[] = {
    {"convert", (PyCFunction) file_converter_convert, METH_VARARGS | METH_KEYWORDS | METH_STATIC, ""},
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
