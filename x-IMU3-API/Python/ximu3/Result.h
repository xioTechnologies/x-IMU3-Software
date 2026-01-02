#ifndef RESULT_H
#define RESULT_H

#include "../../C/Ximu3.h"
#include <Python.h>

static PyObject *result_to_string(PyObject *self, PyObject *args) {
    int result_int;

    if (PyArg_ParseTuple(args, "i", &result_int) == 0) {
        return NULL;
    }

    const XIMU3_Result result = (XIMU3_Result) result_int;

    switch (result) {
        case XIMU3_ResultOk:
        case XIMU3_ResultAddrInUse:
        case XIMU3_ResultAddrNotAvailable:
        case XIMU3_ResultAlreadyExists:
        case XIMU3_ResultArgumentListTooLong:
        case XIMU3_ResultBrokenPipe:
        case XIMU3_ResultConnectionAborted:
        case XIMU3_ResultConnectionRefused:
        case XIMU3_ResultConnectionReset:
        case XIMU3_ResultDeadlock:
        case XIMU3_ResultDirectoryNotEmpty:
        case XIMU3_ResultExecutableFileBusy:
        case XIMU3_ResultFileTooLarge:
        case XIMU3_ResultHostUnreachable:
        case XIMU3_ResultInterrupted:
        case XIMU3_ResultInvalidData:
        case XIMU3_ResultInvalidInput:
        case XIMU3_ResultIsADirectory:
        case XIMU3_ResultNetworkDown:
        case XIMU3_ResultNetworkUnreachable:
        case XIMU3_ResultNotADirectory:
        case XIMU3_ResultNotConnected:
        case XIMU3_ResultNotFound:
        case XIMU3_ResultNotSeekable:
        case XIMU3_ResultOther:
        case XIMU3_ResultOutOfMemory:
        case XIMU3_ResultPermissionDenied:
        case XIMU3_ResultReadOnlyFilesystem:
        case XIMU3_ResultResourceBusy:
        case XIMU3_ResultStaleNetworkFileHandle:
        case XIMU3_ResultStorageFull:
        case XIMU3_ResultTimedOut:
        case XIMU3_ResultTooManyLinks:
        case XIMU3_ResultUnexpectedEof:
        case XIMU3_ResultUnsupported:
        case XIMU3_ResultWouldBlock:
        case XIMU3_ResultWriteZero:
        case XIMU3_ResultUnknownError:
            break;
        default:
            PyErr_SetString(PyExc_ValueError, "Expected RESULT_*");
            return NULL;
    }

    const char *const string = XIMU3_result_to_string(result);

    return PyUnicode_FromString(string);
}

static PyMethodDef result_methods[] = {
    {"result_to_string", (PyCFunction) result_to_string, METH_VARARGS, ""},
    {NULL} /* sentinel */
};

static void result_callback(XIMU3_Result data, void *context) {
    PyObject *object = NULL;
    PyObject *tuple = NULL;
    PyObject *result = NULL;

    const PyGILState_STATE state = PyGILState_Ensure();

    object = PyLong_FromLong((long) data);

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
