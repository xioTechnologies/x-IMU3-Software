#ifndef FILE_CONVERTER_PROGRESS_H
#define FILE_CONVERTER_PROGRESS_H

#include "../../C/Ximu3.h"
#include <Python.h>

typedef struct
{
    PyObject_HEAD
    XIMU3_FileConverterProgress progress;
} FileConverterProgress;

static void file_converter_progress_free(FileConverterProgress* self)
{
    Py_TYPE(self)->tp_free(self);
}

static PyObject* file_converter_progress_get_status(FileConverterProgress* self)
{
    return Py_BuildValue("i", self->progress.status);
}

static PyObject* file_converter_progress_get_percentage(FileConverterProgress* self)
{
    return Py_BuildValue("f", self->progress.percentage);
}

static PyObject* file_converter_progress_get_bytes_processed(FileConverterProgress* self)
{
    return Py_BuildValue("K", self->progress.bytes_processed);
}

static PyObject* file_converter_progress_get_file_size(FileConverterProgress* self)
{
    return Py_BuildValue("K", self->progress.file_size);
}

static PyObject* file_converter_progress_to_string(FileConverterProgress* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_file_converter_progress_to_string(self->progress));
}

static PyGetSetDef file_converter_progress_get_set[] = {
        { "status",          (getter) file_converter_progress_get_status,          NULL, "", NULL },
        { "percentage",      (getter) file_converter_progress_get_percentage,      NULL, "", NULL },
        { "bytes_processed", (getter) file_converter_progress_get_bytes_processed, NULL, "", NULL },
        { "file_size",       (getter) file_converter_progress_get_file_size,       NULL, "", NULL },
        { NULL }  /* sentinel */
};

static PyMethodDef file_converter_progress_methods[] = {
        { "to_string", (PyCFunction) file_converter_progress_to_string, METH_NOARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject file_converter_progress_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.FileConverterProgress",
        .tp_basicsize = sizeof(FileConverterProgress),
        .tp_dealloc = (destructor) file_converter_progress_free,
        .tp_new = PyType_GenericNew,
        .tp_getset = file_converter_progress_get_set,
        .tp_methods = file_converter_progress_methods,
};

static PyObject* file_converter_progress_from(const XIMU3_FileConverterProgress* const progress)
{
    FileConverterProgress* const self = (FileConverterProgress*) file_converter_progress_object.tp_alloc(&file_converter_progress_object, 0);
    self->progress = *progress;
    return (PyObject*) self;
}

static void file_converter_progress_callback(XIMU3_FileConverterProgress data, void* context)
{
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject* const object = file_converter_progress_from(&data);
    PyObject* const tuple = Py_BuildValue("(O)", object);
    Py_DECREF(PyObject_CallObject((PyObject*) context, tuple));
    Py_DECREF(tuple);
    Py_DECREF(object);

    PyGILState_Release(state);
}

#endif
