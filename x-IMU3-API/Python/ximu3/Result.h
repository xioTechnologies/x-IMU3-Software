#ifndef RESULT_H
#define RESULT_H

#include "../../C/Ximu3.h"
#include <Python.h>

static int result_from(XIMU3_Result *const result, const int result_int) {
    switch (result_int) {
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
            *result = (XIMU3_Result) result_int;
            return 0;
    }

    PyErr_SetString(PyExc_ValueError, "'result' must be ximu3.RESULT_*");
    return -1;
}

static PyObject *result_to_string(PyObject *null, PyObject *arg) {
    const int result_int = (int) PyLong_AsLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    XIMU3_Result result;

    if (result_from(&result, result_int) != 0) {
        return NULL;
    }

    const char *const string = XIMU3_result_to_string(result);

    return PyUnicode_FromString(string);
}

static PyMethodDef result_methods[] = {
    {"result_to_string", (PyCFunction) result_to_string, METH_O, ""},
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
