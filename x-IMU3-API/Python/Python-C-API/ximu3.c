#include "../../C/Ximu3.h"
#include "ChargingStatus.h"
#include "CommandMessage.h"
#include "Connection.h"
#include "ConnectionInfo.h"
#include "ConnectionStatus.h"
#include "DataLogger.h"
#include "DataMessages/DataMessages.h"
#include "Device.h"
#include "FileConverter.h"
#include "FileConverterProgress.h"
#include "FileConverterStatus.h"
#include "KeepOpen.h"
#include "NetworkAnnouncement.h"
#include "NetworkAnnouncementMessage.h"
#include "PingResponse.h"
#include "PortScanner.h"
#include "PortType.h"
#include <Python.h>
#include "ReceiveError.h"
#include "Result.h"
#include "Statistics.h"
#include <stdio.h>

static struct PyModuleDef config = {
    PyModuleDef_HEAD_INIT,
    "ximu3",
    "",
    -1,
};

bool add_object(PyObject* const module, PyTypeObject* const type_object, const char* const name)
{
    if (PyType_Ready(type_object) == 0)
    {
        Py_INCREF(type_object);
        if (PyModule_AddObject(module, name, (PyObject*) type_object) == 0)
        {
            return true;
        }
        Py_DECREF(type_object);
        return false;
    }
    return false;
}

PyMODINIT_FUNC PyInit_ximu3()
{
    PyObject* const module = PyModule_Create(&config);

    if (module != NULL &&
        (PyModule_AddIntConstant(module, "CHARGING_STATUS_NOT_CONNECTED", XIMU3_ChargingStatusNotConnected) == 0) &&
        (PyModule_AddIntConstant(module, "CHARGING_STATUS_CHARGING", XIMU3_ChargingStatusCharging) == 0) &&
        (PyModule_AddIntConstant(module, "CHARGING_STATUS_CHARGING_COMPLETE", XIMU3_ChargingStatusChargingComplete) == 0) &&
        (PyModule_AddIntConstant(module, "CHARGING_STATUS_CHARGING_ON_HOLD", XIMU3_ChargingStatusChargingOnHold) == 0) &&
        (PyModule_AddIntConstant(module, "CONNECTION_STATUS_CONNECTED", XIMU3_ConnectionStatusConnected) == 0) &&
        (PyModule_AddIntConstant(module, "CONNECTION_STATUS_RECONNECTING", XIMU3_ConnectionStatusReconnecting) == 0) &&
        (PyModule_AddIntConstant(module, "FILE_CONVERTER_STATUS_COMPLETE", XIMU3_FileConverterStatusComplete) == 0) &&
        (PyModule_AddIntConstant(module, "FILE_CONVERTER_STATUS_FAILED", XIMU3_FileConverterStatusFailed) == 0) &&
        (PyModule_AddIntConstant(module, "FILE_CONVERTER_STATUS_IN_PROGRESS", XIMU3_FileConverterStatusInProgress) == 0) &&
        (PyModule_AddIntConstant(module, "PORT_TYPE_USB", XIMU3_PortTypeUsb) == 0) &&
        (PyModule_AddIntConstant(module, "PORT_TYPE_SERIAL", XIMU3_PortTypeSerial) == 0) &&
        (PyModule_AddIntConstant(module, "PORT_TYPE_BLUETOOTH", XIMU3_PortTypeBluetooth) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_BUFFER_OVERRUN", XIMU3_ReceiveErrorBufferOverrun) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_INVALID_MESSAGE_IDENTIFIER", XIMU3_ReceiveErrorInvalidMessageIdentifier) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_INVALID_UTF8", XIMU3_ReceiveErrorInvalidUtf8) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_INVALID_JSON", XIMU3_ReceiveErrorInvalidJson) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_JSON_IS_NOT_AN_OBJECT", XIMU3_ReceiveErrorJsonIsNotAnObject) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_JSON_OBJECT_IS_NOT_A_SINGLE_KEY_VALUE_PAIR", XIMU3_ReceiveErrorJsonObjectIsNotASingleKeyValuePair) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_INVALID_MUX_MESSAGE_LENGTH", XIMU3_ReceiveErrorInvalidMuxMessageLength) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_INVALID_ESCAPE_SEQUENCE", XIMU3_ReceiveErrorInvalidEscapeSequence) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_INVALID_BINARY_MESSAGE_LENGTH", XIMU3_ReceiveErrorInvalidBinaryMessageLength) == 0) &&
        (PyModule_AddIntConstant(module, "RECEIVE_ERROR_UNABLE_TO_PARSE_ASCII_MESSAGE", XIMU3_ReceiveErrorUnableToParseAsciiMessage) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_OK", XIMU3_ResultOk) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_ADDR_IN_USE", XIMU3_ResultAddrInUse) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_ADDR_NOT_AVAILABLE", XIMU3_ResultAddrNotAvailable) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_ALREADY_EXISTS", XIMU3_ResultAlreadyExists) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_ARGUMENT_LIST_TOO_LONG", XIMU3_ResultArgumentListTooLong) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_BROKEN_PIPE", XIMU3_ResultBrokenPipe) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_CONNECTION_ABORTED", XIMU3_ResultConnectionAborted) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_CONNECTION_REFUSED", XIMU3_ResultConnectionRefused) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_CONNECTION_RESET", XIMU3_ResultConnectionReset) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_DEADLOCK", XIMU3_ResultDeadlock) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_DIRECTORY_NOT_EMPTY", XIMU3_ResultDirectoryNotEmpty) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_EXECUTABLE_FILE_BUSY", XIMU3_ResultExecutableFileBusy) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_FILE_TOO_LARGE", XIMU3_ResultFileTooLarge) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_HOST_UNREACHABLE", XIMU3_ResultHostUnreachable) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_INTERRUPTED", XIMU3_ResultInterrupted) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_INVALID_DATA", XIMU3_ResultInvalidData) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_INVALID_INPUT", XIMU3_ResultInvalidInput) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_IS_A_DIRECTORY", XIMU3_ResultIsADirectory) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_NETWORK_DOWN", XIMU3_ResultNetworkDown) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_NETWORK_UNREACHABLE", XIMU3_ResultNetworkUnreachable) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_NOT_A_DIRECTORY", XIMU3_ResultNotADirectory) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_NOT_CONNECTED", XIMU3_ResultNotConnected) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_NOT_FOUND", XIMU3_ResultNotFound) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_NOT_SEEKABLE", XIMU3_ResultNotSeekable) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_OTHER", XIMU3_ResultOther) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_OUT_OF_MEMORY", XIMU3_ResultOutOfMemory) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_PERMISSION_DENIED", XIMU3_ResultPermissionDenied) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_READ_ONLY_FILESYSTEM", XIMU3_ResultReadOnlyFilesystem) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_RESOURCE_BUSY", XIMU3_ResultResourceBusy) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_STALE_NETWORK_FILE_HANDLE", XIMU3_ResultStaleNetworkFileHandle) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_STORAGE_FULL", XIMU3_ResultStorageFull) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_TIMED_OUT", XIMU3_ResultTimedOut) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_TOO_MANY_LINKS", XIMU3_ResultTooManyLinks) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_UNEXPECTED_EOF", XIMU3_ResultUnexpectedEof) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_UNSUPPORTED", XIMU3_ResultUnsupported) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_WOULD_BLOCK", XIMU3_ResultWouldBlock) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_WRITE_ZERO", XIMU3_ResultWriteZero) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_UNKNOWN_ERROR", XIMU3_ResultUnknownError) == 0) &&
        (PyModule_AddFunctions(module, charging_status_methods) == 0) &&
        (PyModule_AddFunctions(module, connection_status_methods) == 0) &&
        (PyModule_AddFunctions(module, file_converter_status_methods) == 0) &&
        (PyModule_AddFunctions(module, port_type_methods) == 0) &&
        (PyModule_AddFunctions(module, receive_error_methods) == 0) &&
        (PyModule_AddFunctions(module, result_methods) == 0) &&
        add_object(module, &command_message_object, "CommandMessage") &&
        add_object(module, &connection_object, "Connection") &&
        add_object(module, &data_logger_object, "DataLogger") &&
        add_object(module, &device_object, "Device") &&
        add_object(module, &file_converter_object, "FileConverter") &&
        add_object(module, &file_converter_progress_object, "FileConverterProgress") &&
        // Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
        add_object(module, &inertial_message_object, "InertialMessage") &&
        add_object(module, &magnetometer_message_object, "MagnetometerMessage") &&
        add_object(module, &quaternion_message_object, "QuaternionMessage") &&
        add_object(module, &rotation_matrix_message_object, "RotationMatrixMessage") &&
        add_object(module, &euler_angles_message_object, "EulerAnglesMessage") &&
        add_object(module, &linear_acceleration_message_object, "LinearAccelerationMessage") &&
        add_object(module, &earth_acceleration_message_object, "EarthAccelerationMessage") &&
        add_object(module, &ahrs_status_message_object, "AhrsStatusMessage") &&
        add_object(module, &high_g_accelerometer_message_object, "HighGAccelerometerMessage") &&
        add_object(module, &temperature_message_object, "TemperatureMessage") &&
        add_object(module, &battery_message_object, "BatteryMessage") &&
        add_object(module, &rssi_message_object, "RssiMessage") &&
        add_object(module, &serial_accessory_message_object, "SerialAccessoryMessage") &&
        add_object(module, &notification_message_object, "NotificationMessage") &&
        add_object(module, &error_message_object, "ErrorMessage") &&
        // End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
        add_object(module, &keep_open_object, "KeepOpen") &&
        add_object(module, &network_announcement_object, "NetworkAnnouncement") &&
        add_object(module, &network_announcement_message_object, "NetworkAnnouncementMessage") &&
        add_object(module, &ping_response_object, "PingResponse") &&
        add_object(module, &port_scanner_object, "PortScanner") &&
        add_object(module, &statistics_object, "Statistics") &&
        add_object(module, &usb_connection_info_object, "UsbConnectionInfo") &&
        add_object(module, &serial_connection_info_object, "SerialConnectionInfo") &&
        add_object(module, &tcp_connection_info_object, "TcpConnectionInfo") &&
        add_object(module, &udp_connection_info_object, "UdpConnectionInfo") &&
        add_object(module, &bluetooth_connection_info_object, "BluetoothConnectionInfo") &&
        add_object(module, &file_connection_info_object, "FileConnectionInfo") &&
        add_object(module, &mux_connection_info_object, "MuxConnectionInfo"))
    {
        return module;
    }

    Py_DECREF(module);

    return NULL;
}

// This function cannot be in ConnectionInfo.h because this results in a circular reference
PyObject* mux_connection_info_new(PyTypeObject* subtype, PyObject* args, PyObject* keywords)
{
    unsigned char channel;
    PyObject* connection;

    if (PyArg_ParseTuple(args, "bO!", &channel, &connection_object, &connection) == 0)
    {
        PyErr_SetString(PyExc_TypeError, INVALID_ARGUMENTS_STRING);
        return NULL;
    }

    MuxConnectionInfo* const self = (MuxConnectionInfo*) subtype->tp_alloc(subtype, 0);
    self->connection_info = XIMU3_mux_connection_info_new(channel, ((Connection*) connection)->connection);

    return (PyObject*) self;
}
