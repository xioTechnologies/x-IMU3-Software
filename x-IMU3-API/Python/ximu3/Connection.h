#ifndef CONNECTION_H
#define CONNECTION_H

#include "../../C/Ximu3.h"
#include "CommandMessage.h"
#include "ConnectionInfo.h"
#include "DataMessages/DataMessages.h"
#include "PingResponse.h"
#include <Python.h>
#include "ReceiveError.h"
#include "Result.h"
#include "Statistics.h"

#define CHAR_PTR_ARRAY_LENGTH 256

typedef struct {
    PyObject_HEAD
    XIMU3_Connection *connection;
} Connection;

static PyObject *connection_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    PyObject *connection_info;

    if (PyArg_ParseTuple(args, "O", &connection_info)) {
        if (PyObject_TypeCheck(connection_info, &usb_connection_info_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->connection = XIMU3_connection_new_usb(((UsbConnectionInfo *) connection_info)->connection_info);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(connection_info, &serial_connection_info_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->connection = XIMU3_connection_new_serial(((SerialConnectionInfo *) connection_info)->connection_info);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(connection_info, &tcp_connection_info_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->connection = XIMU3_connection_new_tcp(((TcpConnectionInfo *) connection_info)->connection_info);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(connection_info, &udp_connection_info_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->connection = XIMU3_connection_new_udp(((UdpConnectionInfo *) connection_info)->connection_info);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(connection_info, &bluetooth_connection_info_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->connection = XIMU3_connection_new_bluetooth(((BluetoothConnectionInfo *) connection_info)->connection_info);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(connection_info, &file_connection_info_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->connection = XIMU3_connection_new_file(((FileConnectionInfo *) connection_info)->connection_info);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(connection_info, &mux_connection_info_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->connection = XIMU3_connection_new_mux(((MuxConnectionInfo *) connection_info)->connection_info);
            return (PyObject *) self;
        }
    }

    PyErr_SetString(PyExc_TypeError, "'connection_info' must be ximu3.*ConnectionInfo");
    return NULL;
}

static void connection_free(Connection *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_connection_free(self->connection);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *connection_open(Connection *self, PyObject *args) {
    const XIMU3_Result result = XIMU3_connection_open(self->connection);

    return PyLong_FromLong((long) result);
}

static PyObject *connection_open_async(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    XIMU3_connection_open_async(self->connection, result_callback, arg);
    Py_RETURN_NONE;
}

static PyObject *connection_close(Connection *self, PyObject *args) {
    XIMU3_connection_close(self->connection);
    Py_RETURN_NONE;
}

static PyObject *connection_ping(Connection *self, PyObject *args) {
    XIMU3_PingResponse ping_response;

    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        ping_response = XIMU3_connection_ping(self->connection);
    Py_END_ALLOW_THREADS

    return ping_response_from(&ping_response);
}

static PyObject *connection_ping_async(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    XIMU3_connection_ping_async(self->connection, ping_response_callback, arg);
    Py_RETURN_NONE;
}

static PyObject *connection_send_command(Connection *self, PyObject *args) {
    const char *command;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    if (PyArg_ParseTuple(args, "s|kk", &command, &retries, &timeout) == 0) {
        return NULL;
    }

    XIMU3_CommandMessage response;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        response = XIMU3_connection_send_command(self->connection, command, (uint32_t) retries, (uint32_t) timeout);
    Py_END_ALLOW_THREADS

    return command_message_from(&response);
}

static PyObject *connection_send_commands(Connection *self, PyObject *args) {
    PyObject *commands_sequence;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    if (PyArg_ParseTuple(args, "O|kk", &commands_sequence, &retries, &timeout) == 0) {
        return NULL;
    }

    if (PySequence_Check(commands_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, "'commands' must be a sequence");
        return NULL;
    }

    const uint32_t length = (uint32_t) PySequence_Size(commands_sequence);

    if (length > CHAR_PTR_ARRAY_LENGTH) {
        PyErr_Format(PyExc_ValueError, "'commands' has too many items. Cannot exceed %d.", CHAR_PTR_ARRAY_LENGTH);
        return NULL;
    }

    const char *commands[CHAR_PTR_ARRAY_LENGTH];

    for (uint32_t index = 0; index < length; index++) {
        PyObject *command = PySequence_GetItem(commands_sequence, index); // TODO: this will never be destroyed (memory leak)

        commands[index] = (char *) PyUnicode_AsUTF8(command);

        if (commands[index] == NULL) {
            return NULL;
        }
    }

    XIMU3_CommandMessages responses;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        responses = XIMU3_connection_send_commands(self->connection, commands, length, (uint32_t) retries, (uint32_t) timeout);
    Py_END_ALLOW_THREADS

    return command_messages_to_list_and_free(responses);
}

static PyObject *connection_send_command_async(Connection *self, PyObject *args) {
    const char *command;
    PyObject *callable;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    if (PyArg_ParseTuple(args, "sO:set_callback|kk", &command, &callable, &retries, &timeout) == 0) {
        return NULL;
    }

    if (PyCallable_Check(callable) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(callable); // TODO: this will never be destroyed (memory leak)

    XIMU3_connection_send_command_async(self->connection, command, (uint32_t) retries, (uint32_t) timeout, command_message_callback, callable);
    Py_RETURN_NONE;
}

static PyObject *connection_send_commands_async(Connection *self, PyObject *args) {
    PyObject *commands_sequence;
    PyObject *callable;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    if (PyArg_ParseTuple(args, "OO:set_callback|kk", &commands_sequence, &callable, &retries, &timeout) == 0) {
        return NULL;
    }

    if (PySequence_Check(commands_sequence) == 0) {
        PyErr_SetString(PyExc_TypeError, "'commands' must be a sequence");
        return NULL;
    }

    const uint32_t length = (uint32_t) PySequence_Size(commands_sequence);

    if (length > CHAR_PTR_ARRAY_LENGTH) {
        PyErr_Format(PyExc_ValueError, "'commands' has too many items. Cannot exceed %d.", CHAR_PTR_ARRAY_LENGTH);
        return NULL;
    }

    const char *commands[CHAR_PTR_ARRAY_LENGTH];

    for (uint32_t index = 0; index < length; index++) {
        PyObject *command = PySequence_GetItem(commands_sequence, index); // TODO: this will never be destroyed (memory leak)

        commands[index] = (char *) PyUnicode_AsUTF8(command);

        if (commands[index] == NULL) {
            return NULL;
        }
    }

    if (PyCallable_Check(callable) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(callable); // TODO: this will never be destroyed (memory leak)

    XIMU3_connection_send_commands_async(self->connection, commands, length, (uint32_t) retries, (uint32_t) timeout, command_messages_callback, callable);
    Py_RETURN_NONE;
}

static PyObject *connection_get_info(Connection *self, PyObject *args) {
    switch (XIMU3_connection_get_type(self->connection)) {
        case XIMU3_ConnectionTypeUsb: {
            const XIMU3_UsbConnectionInfo connection_info = XIMU3_connection_get_info_usb(self->connection);

            return usb_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeSerial: {
            const XIMU3_SerialConnectionInfo connection_info = XIMU3_connection_get_info_serial(self->connection);

            return serial_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeTcp: {
            const XIMU3_TcpConnectionInfo connection_info = XIMU3_connection_get_info_tcp(self->connection);

            return tcp_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeUdp: {
            const XIMU3_UdpConnectionInfo connection_info = XIMU3_connection_get_info_udp(self->connection);

            return udp_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeBluetooth: {
            const XIMU3_BluetoothConnectionInfo connection_info = XIMU3_connection_get_info_bluetooth(self->connection);

            return bluetooth_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeFile: {
            const XIMU3_FileConnectionInfo connection_info = XIMU3_connection_get_info_file(self->connection);

            return file_connection_info_from(&connection_info);
        }
        case XIMU3_ConnectionTypeMux: {
            XIMU3_MuxConnectionInfo *connection_info = XIMU3_connection_get_info_mux(self->connection);

            return mux_connection_info_from(connection_info);
        }
    }
    return NULL;
}

static PyObject *connection_get_statistics(Connection *self, PyObject *args) {
    const XIMU3_Statistics statistics = XIMU3_connection_get_statistics(self->connection);

    return statistics_from(&statistics);
}

static PyObject *connection_add_receive_error_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_receive_error_callback(self->connection, receive_error_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_statistics_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_statistics_callback(self->connection, statistics_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

// Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
static PyObject *connection_add_inertial_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_inertial_callback(self->connection, inertial_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_magnetometer_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_magnetometer_callback(self->connection, magnetometer_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_quaternion_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_quaternion_callback(self->connection, quaternion_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_rotation_matrix_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_rotation_matrix_callback(self->connection, rotation_matrix_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_euler_angles_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_euler_angles_callback(self->connection, euler_angles_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_linear_acceleration_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_linear_acceleration_callback(self->connection, linear_acceleration_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_earth_acceleration_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_earth_acceleration_callback(self->connection, earth_acceleration_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_ahrs_status_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_ahrs_status_callback(self->connection, ahrs_status_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_high_g_accelerometer_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_high_g_accelerometer_callback(self->connection, high_g_accelerometer_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_temperature_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_temperature_callback(self->connection, temperature_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_battery_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_battery_callback(self->connection, battery_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_rssi_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_rssi_callback(self->connection, rssi_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_serial_accessory_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_serial_accessory_callback(self->connection, serial_accessory_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_notification_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_notification_callback(self->connection, notification_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_add_error_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_error_callback(self->connection, error_message_callback, arg);
    Py_END_ALLOW_THREADS

    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

// End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

static void end_of_file_callback(void *context) {
    const PyGILState_STATE state = PyGILState_Ensure();

    PyObject *const result = PyObject_CallObject((PyObject *) context, NULL);

    if (result == NULL) {
        PyErr_Print();
    }
    Py_XDECREF(result);

    PyGILState_Release(state);
}

static PyObject *connection_add_end_of_file_callback(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    uint64_t id;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        id = XIMU3_connection_add_end_of_file_callback(self->connection, end_of_file_callback, arg);
    Py_END_ALLOW_THREADS
    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_remove_callback(Connection *self, PyObject *arg) {
    const unsigned long long callback_id = PyLong_AsUnsignedLongLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_connection_remove_callback(self->connection, (uint64_t) callback_id);
    Py_END_ALLOW_THREADS

    Py_RETURN_NONE;
}

static PyMethodDef connection_methods[] = {
    {"open", (PyCFunction) connection_open, METH_NOARGS, ""},
    {"open_async", (PyCFunction) connection_open_async, METH_O, ""},
    {"close", (PyCFunction) connection_close, METH_NOARGS, ""},
    {"ping", (PyCFunction) connection_ping, METH_NOARGS, ""},
    {"ping_async", (PyCFunction) connection_ping_async, METH_O, ""},
    {"send_command", (PyCFunction) connection_send_command, METH_VARARGS, ""},
    {"send_commands", (PyCFunction) connection_send_commands, METH_VARARGS, ""},
    {"send_command_async", (PyCFunction) connection_send_command_async, METH_VARARGS, ""},
    {"send_commands_async", (PyCFunction) connection_send_commands_async, METH_VARARGS, ""},
    {"get_info", (PyCFunction) connection_get_info, METH_NOARGS, ""},
    {"get_statistics", (PyCFunction) connection_get_statistics, METH_NOARGS, ""},
    {"add_receive_error_callback", (PyCFunction) connection_add_receive_error_callback, METH_O, ""},
    {"add_statistics_callback", (PyCFunction) connection_add_statistics_callback, METH_O, ""},
    // Start of code block #1 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
    {"add_inertial_callback", (PyCFunction) connection_add_inertial_callback, METH_O, ""},
    {"add_magnetometer_callback", (PyCFunction) connection_add_magnetometer_callback, METH_O, ""},
    {"add_quaternion_callback", (PyCFunction) connection_add_quaternion_callback, METH_O, ""},
    {"add_rotation_matrix_callback", (PyCFunction) connection_add_rotation_matrix_callback, METH_O, ""},
    {"add_euler_angles_callback", (PyCFunction) connection_add_euler_angles_callback, METH_O, ""},
    {"add_linear_acceleration_callback", (PyCFunction) connection_add_linear_acceleration_callback, METH_O, ""},
    {"add_earth_acceleration_callback", (PyCFunction) connection_add_earth_acceleration_callback, METH_O, ""},
    {"add_ahrs_status_callback", (PyCFunction) connection_add_ahrs_status_callback, METH_O, ""},
    {"add_high_g_accelerometer_callback", (PyCFunction) connection_add_high_g_accelerometer_callback, METH_O, ""},
    {"add_temperature_callback", (PyCFunction) connection_add_temperature_callback, METH_O, ""},
    {"add_battery_callback", (PyCFunction) connection_add_battery_callback, METH_O, ""},
    {"add_rssi_callback", (PyCFunction) connection_add_rssi_callback, METH_O, ""},
    {"add_serial_accessory_callback", (PyCFunction) connection_add_serial_accessory_callback, METH_O, ""},
    {"add_notification_callback", (PyCFunction) connection_add_notification_callback, METH_O, ""},
    {"add_error_callback", (PyCFunction) connection_add_error_callback, METH_O, ""},
    // End of code block #1 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
    {"add_end_of_file_callback", (PyCFunction) connection_add_end_of_file_callback, METH_O, ""},
    {"remove_callback", (PyCFunction) connection_remove_callback, METH_O, ""},
    {NULL} /* sentinel */
};

static PyTypeObject connection_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ximu3.Connection",
    .tp_basicsize = sizeof(Connection),
    .tp_dealloc = (destructor) connection_free,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = connection_new,
    .tp_methods = connection_methods,
};

#endif
