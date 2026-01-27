#ifndef CONNECTION_H
#define CONNECTION_H

#include "../../C/Ximu3.h"
#include "CommandMessage.h"
#include "ConnectionConfig.h"
#include "DataMessages/DataMessages.h"
#include "PingResponse.h"
#include <Python.h>
#include "ReceiveError.h"
#include "Result.h"
#include "Statistics.h"

#define CHAR_PTR_ARRAY_LENGTH 256

typedef struct {
    PyObject_HEAD
    XIMU3_Connection *wrapped;
} Connection;

static PyObject *connection_new(PyTypeObject *subtype, PyObject *args, PyObject *kwds) {
    PyObject *config;

    static char *kwlist[] = {
        "config",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &config)) {
        if (PyObject_TypeCheck(config, &usb_connection_config_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->wrapped = XIMU3_connection_new_usb(((UsbConnectionConfig *) config)->wrapped);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(config, &serial_connection_config_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->wrapped = XIMU3_connection_new_serial(((SerialConnectionConfig *) config)->wrapped);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(config, &tcp_connection_config_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->wrapped = XIMU3_connection_new_tcp(((TcpConnectionConfig *) config)->wrapped);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(config, &udp_connection_config_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->wrapped = XIMU3_connection_new_udp(((UdpConnectionConfig *) config)->wrapped);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(config, &bluetooth_connection_config_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->wrapped = XIMU3_connection_new_bluetooth(((BluetoothConnectionConfig *) config)->wrapped);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(config, &file_connection_config_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->wrapped = XIMU3_connection_new_file(((FileConnectionConfig *) config)->wrapped);
            return (PyObject *) self;
        }
        if (PyObject_TypeCheck(config, &mux_connection_config_object) != 0) {
            Connection *self = (Connection *) subtype->tp_alloc(subtype, 0);

            if (self == NULL) {
                return NULL;
            }

            self->wrapped = XIMU3_connection_new_mux(((MuxConnectionConfig *) config)->wrapped);
            return (PyObject *) self;
        }
    }

    PyErr_SetString(PyExc_TypeError, "'config' must be ximu3.*ConnectionConfig");
    return NULL;
}

static void connection_free(Connection *self) {
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_connection_free(self->wrapped);
    Py_END_ALLOW_THREADS
    Py_TYPE(self)->tp_free(self);
}

static PyObject *connection_open(Connection *self, PyObject *args) {
    const XIMU3_Result result = XIMU3_connection_open(self->wrapped);

    if (result != XIMU3_ResultOk) {
        const char *const config = XIMU3_connection_get_config_string(self->wrapped);
        const char *const result_string = XIMU3_result_to_string(result);

        PyErr_Format(PyExc_RuntimeError, "Unable to open %s. %s.", config, result_string);
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *connection_open_async(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    XIMU3_connection_open_async(self->wrapped, result_callback, arg);
    Py_RETURN_NONE;
}

static PyObject *connection_close(Connection *self, PyObject *args) {
    XIMU3_connection_close(self->wrapped);
    Py_RETURN_NONE;
}

static PyObject *connection_ping(Connection *self, PyObject *args) {
    XIMU3_PingResponse response;

    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        response = XIMU3_connection_ping(self->wrapped);
    Py_END_ALLOW_THREADS

    return ping_response_from(&response);
}

static PyObject *connection_ping_async(Connection *self, PyObject *arg) {
    if (PyCallable_Check(arg) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(arg); // TODO: this will never be destroyed (memory leak)

    XIMU3_connection_ping_async(self->wrapped, ping_response_callback, arg);
    Py_RETURN_NONE;
}

static PyObject *connection_send_command(Connection *self, PyObject *args, PyObject *kwds) {
    const char *command;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    static char *kwlist[] = {
        "command",
        "retries",
        "timeout",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "s|kk", kwlist, &command, &retries, &timeout) == 0) {
        return NULL;
    }

    XIMU3_CommandMessage response;
    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        response = XIMU3_connection_send_command(self->wrapped, command, (uint32_t) retries, (uint32_t) timeout);
    Py_END_ALLOW_THREADS

    return command_message_from(&response);
}

static PyObject *connection_send_commands(Connection *self, PyObject *args, PyObject *kwds) {
    PyObject *commands_sequence;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    static char *kwlist[] = {
        "commands",
        "retries",
        "timeout",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "O|kk", kwlist, &commands_sequence, &retries, &timeout) == 0) {
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
        responses = XIMU3_connection_send_commands(self->wrapped, commands, length, (uint32_t) retries, (uint32_t) timeout);
    Py_END_ALLOW_THREADS

    return command_messages_to_list_and_free(responses);
}

static PyObject *connection_send_command_async(Connection *self, PyObject *args, PyObject *kwds) {
    const char *command;
    PyObject *callback;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    static char *kwlist[] = {
        "command",
        "callback",
        "retries",
        "timeout",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "sO|kk", kwlist, &command, &callback, &retries, &timeout) == 0) {
        return NULL;
    }

    if (PyCallable_Check(callback) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(callback); // TODO: this will never be destroyed (memory leak)

    XIMU3_connection_send_command_async(self->wrapped, command, (uint32_t) retries, (uint32_t) timeout, command_message_callback, callback);
    Py_RETURN_NONE;
}

static PyObject *connection_send_commands_async(Connection *self, PyObject *args, PyObject *kwds) {
    PyObject *commands_sequence;
    PyObject *callback;
    unsigned long retries = XIMU3_DEFAULT_RETRIES;
    unsigned long timeout = XIMU3_DEFAULT_TIMEOUT;

    static char *kwlist[] = {
        "commands",
        "callback",
        "retries",
        "timeout",
        NULL, /* sentinel */
    };

    if (PyArg_ParseTupleAndKeywords(args, kwds, "OO|kk", kwlist, &commands_sequence, &callback, &retries, &timeout) == 0) {
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

    if (PyCallable_Check(callback) == 0) {
        PyErr_SetString(PyExc_TypeError, "'callback' must be callable");
        return NULL;
    }

    Py_INCREF(callback); // TODO: this will never be destroyed (memory leak)

    XIMU3_connection_send_commands_async(self->wrapped, commands, length, (uint32_t) retries, (uint32_t) timeout, command_messages_callback, callback);
    Py_RETURN_NONE;
}

static PyObject *connection_get_config(Connection *self, PyObject *args) {
    switch (XIMU3_connection_get_type(self->wrapped)) {
        case XIMU3_ConnectionTypeUsb: {
            const XIMU3_UsbConnectionConfig config = XIMU3_connection_get_config_usb(self->wrapped);

            return usb_connection_config_from(&config);
        }
        case XIMU3_ConnectionTypeSerial: {
            const XIMU3_SerialConnectionConfig config = XIMU3_connection_get_config_serial(self->wrapped);

            return serial_connection_config_from(&config);
        }
        case XIMU3_ConnectionTypeTcp: {
            const XIMU3_TcpConnectionConfig config = XIMU3_connection_get_config_tcp(self->wrapped);

            return tcp_connection_config_from(&config);
        }
        case XIMU3_ConnectionTypeUdp: {
            const XIMU3_UdpConnectionConfig config = XIMU3_connection_get_config_udp(self->wrapped);

            return udp_connection_config_from(&config);
        }
        case XIMU3_ConnectionTypeBluetooth: {
            const XIMU3_BluetoothConnectionConfig config = XIMU3_connection_get_config_bluetooth(self->wrapped);

            return bluetooth_connection_config_from(&config);
        }
        case XIMU3_ConnectionTypeFile: {
            const XIMU3_FileConnectionConfig config = XIMU3_connection_get_config_file(self->wrapped);

            return file_connection_config_from(&config);
        }
        case XIMU3_ConnectionTypeMux: {
            XIMU3_MuxConnectionConfig *config = XIMU3_connection_get_config_mux(self->wrapped);

            return mux_connection_config_from(config);
        }
    }
    return NULL;
}

static PyObject *connection_get_statistics(Connection *self, PyObject *args) {
    const XIMU3_Statistics statistics = XIMU3_connection_get_statistics(self->wrapped);

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
        id = XIMU3_connection_add_receive_error_callback(self->wrapped, receive_error_callback, arg);
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
        id = XIMU3_connection_add_statistics_callback(self->wrapped, statistics_callback, arg);
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
        id = XIMU3_connection_add_inertial_callback(self->wrapped, inertial_message_callback, arg);
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
        id = XIMU3_connection_add_magnetometer_callback(self->wrapped, magnetometer_message_callback, arg);
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
        id = XIMU3_connection_add_quaternion_callback(self->wrapped, quaternion_message_callback, arg);
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
        id = XIMU3_connection_add_rotation_matrix_callback(self->wrapped, rotation_matrix_message_callback, arg);
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
        id = XIMU3_connection_add_euler_angles_callback(self->wrapped, euler_angles_message_callback, arg);
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
        id = XIMU3_connection_add_linear_acceleration_callback(self->wrapped, linear_acceleration_message_callback, arg);
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
        id = XIMU3_connection_add_earth_acceleration_callback(self->wrapped, earth_acceleration_message_callback, arg);
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
        id = XIMU3_connection_add_ahrs_status_callback(self->wrapped, ahrs_status_message_callback, arg);
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
        id = XIMU3_connection_add_high_g_accelerometer_callback(self->wrapped, high_g_accelerometer_message_callback, arg);
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
        id = XIMU3_connection_add_temperature_callback(self->wrapped, temperature_message_callback, arg);
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
        id = XIMU3_connection_add_battery_callback(self->wrapped, battery_message_callback, arg);
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
        id = XIMU3_connection_add_rssi_callback(self->wrapped, rssi_message_callback, arg);
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
        id = XIMU3_connection_add_serial_accessory_callback(self->wrapped, serial_accessory_message_callback, arg);
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
        id = XIMU3_connection_add_notification_callback(self->wrapped, notification_message_callback, arg);
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
        id = XIMU3_connection_add_error_callback(self->wrapped, error_message_callback, arg);
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
        id = XIMU3_connection_add_end_of_file_callback(self->wrapped, end_of_file_callback, arg);
    Py_END_ALLOW_THREADS
    return PyLong_FromUnsignedLongLong((unsigned long long) id);
}

static PyObject *connection_remove_callback(Connection *self, PyObject *arg) {
    const unsigned long long callback_id = PyLong_AsUnsignedLongLong(arg);

    if (PyErr_Occurred()) {
        return NULL;
    }

    Py_BEGIN_ALLOW_THREADS // avoid deadlock caused by PyGILState_Ensure in callbacks
        XIMU3_connection_remove_callback(self->wrapped, (uint64_t) callback_id);
    Py_END_ALLOW_THREADS

    Py_RETURN_NONE;
}

static PyMethodDef connection_methods[] = {
    {"open", (PyCFunction) connection_open, METH_NOARGS, ""},
    {"open_async", (PyCFunction) connection_open_async, METH_O, ""},
    {"close", (PyCFunction) connection_close, METH_NOARGS, ""},
    {"ping", (PyCFunction) connection_ping, METH_NOARGS, ""},
    {"ping_async", (PyCFunction) connection_ping_async, METH_O, ""},
    {"send_command", (PyCFunction) connection_send_command, METH_VARARGS | METH_KEYWORDS, ""},
    {"send_commands", (PyCFunction) connection_send_commands, METH_VARARGS | METH_KEYWORDS, ""},
    {"send_command_async", (PyCFunction) connection_send_command_async, METH_VARARGS | METH_KEYWORDS, ""},
    {"send_commands_async", (PyCFunction) connection_send_commands_async, METH_VARARGS | METH_KEYWORDS, ""},
    {"get_config", (PyCFunction) connection_get_config, METH_NOARGS, ""},
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
