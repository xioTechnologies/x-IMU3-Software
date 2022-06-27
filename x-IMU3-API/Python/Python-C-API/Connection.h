#ifndef CONNECTION_H
#define CONNECTION_H

#include "../../C/Ximu3.h"
#include "CharArraysCallback.h"
#include "ConnectionInfo.h"
#include "DataMessages/DataMessages.h"
#include "DecodeErrorCallback.h"
#include "Helpers.h"
#include "PingResponse.h"
#include <Python.h>
#include "ResultCallback.h"
#include "Statistics.h"

#define CHAR_PTR_ARRAY_LENGTH 256

typedef struct
{
    PyObject_HEAD
    XIMU3_Connection* connection;
} Connection;

static PyObject* connection_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
{
    Connection* const self = (Connection*) subtype->tp_alloc(subtype, 0);

    PyObject* connection_info;

    if (PyArg_ParseTuple(args, "O", &connection_info))
    {
        if (PyObject_IsInstance(connection_info, (PyObject*) &usb_connection_info_object))
        {
            self->connection = XIMU3_connection_new_usb(((UsbConnectionInfo*) connection_info)->connection_info);
            return (PyObject*) self;
        }
        if (PyObject_IsInstance(connection_info, (PyObject*) &serial_connection_info_object))
        {
            self->connection = XIMU3_connection_new_serial(((SerialConnectionInfo*) connection_info)->connection_info);
            return (PyObject*) self;
        }
        if (PyObject_IsInstance(connection_info, (PyObject*) &tcp_connection_info_object))
        {
            self->connection = XIMU3_connection_new_tcp(((TcpConnectionInfo*) connection_info)->connection_info);
            return (PyObject*) self;
        }
        if (PyObject_IsInstance(connection_info, (PyObject*) &udp_connection_info_object))
        {
            self->connection = XIMU3_connection_new_udp(((UdpConnectionInfo*) connection_info)->connection_info);
            return (PyObject*) self;
        }
        if (PyObject_IsInstance(connection_info, (PyObject*) &bluetooth_connection_info_object))
        {
            self->connection = XIMU3_connection_new_bluetooth(((BluetoothConnectionInfo*) connection_info)->connection_info);
            return (PyObject*) self;
        }
        if (PyObject_IsInstance(connection_info, (PyObject*) &file_connection_info_object))
        {
            self->connection = XIMU3_connection_new_file(((FileConnectionInfo*) connection_info)->connection_info);
            return (PyObject*) self;
        }
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    Py_DECREF(self);
    return NULL;
}

static void connection_free(Connection* self)
{
    XIMU3_connection_free(self->connection);
    Py_TYPE(self)->tp_free(self);
}

static PyObject* connection_open(Connection* self, PyObject* args)
{
    return Py_BuildValue("s", XIMU3_result_to_string(XIMU3_connection_open(self->connection)));
}

static PyObject* connection_open_async(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    XIMU3_connection_open_async(self->connection, result_callback, callable);

    Py_IncRef(Py_None);
    return Py_None;
}

static PyObject* connection_close(Connection* self, PyObject* args)
{
    XIMU3_connection_close(self->connection);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* connection_ping(Connection* self, PyObject* args)
{
    const XIMU3_PingResponse ping_response = XIMU3_connection_ping(self->connection);
    return ping_response_from(&ping_response);
}

static PyObject* connection_send_commands(Connection* self, PyObject* args)
{
    PyObject* commands_list;
    unsigned long retries;
    unsigned long timeout;

    if (PyArg_ParseTuple(args, "O!kk", &PyList_Type, &commands_list, &retries, &timeout) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const char* commands_char_ptr_array[CHAR_PTR_ARRAY_LENGTH];
    const uint32_t length = (uint32_t) PyList_Size(commands_list);

    for (uint32_t index = 0; index < length; index++)
    {
        if (index >= CHAR_PTR_ARRAY_LENGTH)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        PyObject* command = PyList_GetItem(commands_list, index);

        if (PyUnicode_Check(command) == 0)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        commands_char_ptr_array[index] = (char*) PyUnicode_AsUTF8(command);
    }

    return char_arrays_to_list_and_free(XIMU3_connection_send_commands(self->connection, commands_char_ptr_array, length, (uint32_t) retries, (uint32_t) timeout));
}

static PyObject* connection_send_commands_async(Connection* self, PyObject* args)
{
    PyObject* commands_list;
    unsigned long retries;
    unsigned long timeout;
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O!kkO:set_callback", &PyList_Type, &commands_list, &retries, &timeout, &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    const char* commands_char_ptr_array[CHAR_PTR_ARRAY_LENGTH];
    const uint32_t length = (uint32_t) PyList_Size(commands_list);

    for (uint32_t index = 0; index < length; index++)
    {
        if (index >= CHAR_PTR_ARRAY_LENGTH)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        PyObject* command = PyList_GetItem(commands_list, index);

        if (PyUnicode_Check(command) == 0)
        {
            PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
            return NULL;
        }

        commands_char_ptr_array[index] = (char*) PyUnicode_AsUTF8(command);
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    XIMU3_connection_send_commands_async(self->connection, commands_char_ptr_array, length, (uint32_t) retries, (uint32_t) timeout, char_arrays_callback, callable);

    Py_IncRef(Py_None);
    return Py_None;
}

static PyObject* connection_get_info(Connection* self, PyObject* args)
{
    switch (XIMU3_connection_get_type(self->connection))
    {
        case XIMU3_ConnectionTypeUsb:
        {
            const XIMU3_UsbConnectionInfo connection_info = XIMU3_connection_get_info_usb(self->connection);
            return usb_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeSerial:
        {
            const XIMU3_SerialConnectionInfo connection_info = XIMU3_connection_get_info_serial(self->connection);
            return serial_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeTcp:
        {
            const XIMU3_TcpConnectionInfo connection_info = XIMU3_connection_get_info_tcp(self->connection);
            return tcp_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeUdp:
        {
            const XIMU3_UdpConnectionInfo connection_info = XIMU3_connection_get_info_udp(self->connection);
            return udp_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeBluetooth:
        {
            const XIMU3_BluetoothConnectionInfo connection_info = XIMU3_connection_get_info_bluetooth(self->connection);
            return bluetooth_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeFile:
        {
            const XIMU3_FileConnectionInfo connection_info = XIMU3_connection_get_info_file(self->connection);
            return file_connection_info_from(&connection_info);
        }
    }
    return NULL;
}

static PyObject* connection_get_statistics(Connection* self, PyObject* args)
{
    const XIMU3_Statistics statistics = XIMU3_connection_get_statistics(self->connection);
    return statistics_from(&statistics);
}

static PyObject* connection_add_decode_error_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_decode_error_callback(self->connection, decode_error_callback, callable));
}

static PyObject* connection_add_statistics_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_statistics_callback(self->connection, statistics_callback, callable));
}

// Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

static PyObject* connection_add_inertial_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_inertial_callback(self->connection, inertial_message_callback, callable));
}

static PyObject* connection_add_magnetometer_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_magnetometer_callback(self->connection, magnetometer_message_callback, callable));
}

static PyObject* connection_add_quaternion_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_quaternion_callback(self->connection, quaternion_message_callback, callable));
}

static PyObject* connection_add_rotation_matrix_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_rotation_matrix_callback(self->connection, rotation_matrix_message_callback, callable));
}

static PyObject* connection_add_euler_angles_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_euler_angles_callback(self->connection, euler_angles_message_callback, callable));
}

static PyObject* connection_add_linear_acceleration_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_linear_acceleration_callback(self->connection, linear_acceleration_message_callback, callable));
}

static PyObject* connection_add_earth_acceleration_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_earth_acceleration_callback(self->connection, earth_acceleration_message_callback, callable));
}

static PyObject* connection_add_high_g_accelerometer_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_high_g_accelerometer_callback(self->connection, high_g_accelerometer_message_callback, callable));
}

static PyObject* connection_add_temperature_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_temperature_callback(self->connection, temperature_message_callback, callable));
}

static PyObject* connection_add_battery_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_battery_callback(self->connection, battery_message_callback, callable));
}

static PyObject* connection_add_rssi_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_rssi_callback(self->connection, rssi_message_callback, callable));
}

static PyObject* connection_add_serial_accessory_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_serial_accessory_callback(self->connection, serial_accessory_message_callback, callable));
}

static PyObject* connection_add_notification_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_notification_callback(self->connection, notification_message_callback, callable));
}

static PyObject* connection_add_error_callback(Connection* self, PyObject* args)
{
    PyObject* callable;

    if (PyArg_ParseTuple(args, "O:set_callback", &callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    if (PyCallable_Check(callable) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    Py_INCREF(callable); // this will never be destroyed (memory leak)

    return Py_BuildValue("K", XIMU3_connection_add_error_callback(self->connection, error_message_callback, callable));
}
// End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

static PyObject* connection_remove_callback(Connection* self, PyObject* args)
{
    unsigned long long callback_id;
    if (PyArg_ParseTuple(args, "K", &callback_id))
    {
        XIMU3_connection_remove_callback(self->connection, (uint64_t) callback_id);

        Py_INCREF(Py_None);
        return Py_None;
    }

    PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
    return NULL;
}

static PyMethodDef connection_methods[] = {
        { "open",                              (PyCFunction) connection_open,                              METH_NOARGS,  "" },
        { "open_async",                        (PyCFunction) connection_open_async,                        METH_VARARGS, "" },
        { "close",                             (PyCFunction) connection_close,                             METH_NOARGS,  "" },
        { "ping",                              (PyCFunction) connection_ping,                              METH_NOARGS,  "" },
        { "send_commands",                     (PyCFunction) connection_send_commands,                     METH_VARARGS, "" },
        { "send_commands_async",               (PyCFunction) connection_send_commands_async,               METH_VARARGS, "" },
        { "get_info",                          (PyCFunction) connection_get_info,                          METH_NOARGS,  "" },
        { "get_statistics",                    (PyCFunction) connection_get_statistics,                    METH_NOARGS,  "" },
        { "add_decode_error_callback",         (PyCFunction) connection_add_decode_error_callback,         METH_VARARGS, "" },
        { "add_statistics_callback",           (PyCFunction) connection_add_statistics_callback,           METH_VARARGS, "" },
        // Start of code block #1 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
        { "add_inertial_callback",             (PyCFunction) connection_add_inertial_callback,             METH_VARARGS, "" },
        { "add_magnetometer_callback",         (PyCFunction) connection_add_magnetometer_callback,         METH_VARARGS, "" },
        { "add_quaternion_callback",           (PyCFunction) connection_add_quaternion_callback,           METH_VARARGS, "" },
        { "add_rotation_matrix_callback",      (PyCFunction) connection_add_rotation_matrix_callback,      METH_VARARGS, "" },
        { "add_euler_angles_callback",         (PyCFunction) connection_add_euler_angles_callback,         METH_VARARGS, "" },
        { "add_linear_acceleration_callback",  (PyCFunction) connection_add_linear_acceleration_callback,  METH_VARARGS, "" },
        { "add_earth_acceleration_callback",   (PyCFunction) connection_add_earth_acceleration_callback,   METH_VARARGS, "" },
        { "add_high_g_accelerometer_callback", (PyCFunction) connection_add_high_g_accelerometer_callback, METH_VARARGS, "" },
        { "add_temperature_callback",          (PyCFunction) connection_add_temperature_callback,          METH_VARARGS, "" },
        { "add_battery_callback",              (PyCFunction) connection_add_battery_callback,              METH_VARARGS, "" },
        { "add_rssi_callback",                 (PyCFunction) connection_add_rssi_callback,                 METH_VARARGS, "" },
        { "add_serial_accessory_callback",     (PyCFunction) connection_add_serial_accessory_callback,     METH_VARARGS, "" },
        { "add_notification_callback",         (PyCFunction) connection_add_notification_callback,         METH_VARARGS, "" },
        { "add_error_callback",                (PyCFunction) connection_add_error_callback,                METH_VARARGS, "" },
        // End of code block #1 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
        { "remove_callback",                   (PyCFunction) connection_remove_callback,                   METH_VARARGS, "" },
        { NULL } /* sentinel */
};

static PyTypeObject connection_object = {
        PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "ximu3.Connection",
        .tp_basicsize = sizeof(Connection),
        .tp_dealloc = (destructor) connection_free,
        .tp_new = connection_new,
        .tp_methods = connection_methods,
};

#endif
