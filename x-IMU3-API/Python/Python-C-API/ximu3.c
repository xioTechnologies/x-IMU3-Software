#include "../../C/Ximu3.h"
#include "ChargingStatus.h"
#include "CommandMessage.h"
#include "Connection.h"
#include "ConnectionInfo.h"
#include "ConnectionStatus.h"
#include "ConnectionType.h"
#include "DataLogger.h"
#include "DataMessages/DataMessages.h"
#include "DecodeError.h"
#include "Device.h"
#include "FileConverter.h"
#include "FileConverterProgress.h"
#include "FileConverterStatus.h"
#include "KeepOpen.h"
#include "NetworkAnnouncement.h"
#include "NetworkAnnouncementMessage.h"
#include "PingResponse.h"
#include "PortScanner.h"
#include <Python.h>
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
        (PyModule_AddIntConstant(module, "CONNECTION_TYPE_USB", XIMU3_ConnectionTypeUsb) == 0) &&
        (PyModule_AddIntConstant(module, "CONNECTION_TYPE_SERIAL", XIMU3_ConnectionTypeSerial) == 0) &&
        (PyModule_AddIntConstant(module, "CONNECTION_TYPE_TCP", XIMU3_ConnectionTypeTcp) == 0) &&
        (PyModule_AddIntConstant(module, "CONNECTION_TYPE_UDP", XIMU3_ConnectionTypeUdp) == 0) &&
        (PyModule_AddIntConstant(module, "CONNECTION_TYPE_BLUETOOTH", XIMU3_ConnectionTypeBluetooth) == 0) &&
        (PyModule_AddIntConstant(module, "CONNECTION_TYPE_FILE", XIMU3_ConnectionTypeFile) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_BUFFER_OVERRUN", XIMU3_DecodeErrorBufferOverrun) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_INVALID_MESSAGE_IDENTIFIER", XIMU3_DecodeErrorInvalidMessageIdentifier) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_INVALID_UTF8", XIMU3_DecodeErrorInvalidUtf8) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_INVALID_JSON", XIMU3_DecodeErrorInvalidJson) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_JSON_IS_NOT_AN_OBJECT", XIMU3_DecodeErrorJsonIsNotAnObject) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_JSON_OBJECT_IS_NOT_A_SINGLE_KEY_VALUE_PAIR", XIMU3_DecodeErrorJsonObjectIsNotASingleKeyValuePair) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_INVALID_ESCAPE_SEQUENCE", XIMU3_DecodeErrorInvalidEscapeSequence) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_INVALID_BINARY_MESSAGE_LENGTH", XIMU3_DecodeErrorInvalidBinaryMessageLength) == 0) &&
        (PyModule_AddIntConstant(module, "DECODE_ERROR_UNABLE_TO_PARSE_ASCII_MESSAGE", XIMU3_DecodeErrorUnableToParseAsciiMessage) == 0) &&
        (PyModule_AddIntConstant(module, "FILE_CONVERTER_STATUS_COMPLETE", XIMU3_FileConverterStatusComplete) == 0) &&
        (PyModule_AddIntConstant(module, "FILE_CONVERTER_STATUS_FAILED", XIMU3_FileConverterStatusFailed) == 0) &&
        (PyModule_AddIntConstant(module, "FILE_CONVERTER_STATUS_IN_PROGRESS", XIMU3_FileConverterStatusInProgress) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_OK", XIMU3_ResultOk) == 0) &&
        (PyModule_AddIntConstant(module, "RESULT_ERROR", XIMU3_ResultError) == 0) &&
        (PyModule_AddFunctions(module, charging_status_methods) == 0) &&
        (PyModule_AddFunctions(module, connection_status_methods) == 0) &&
        (PyModule_AddFunctions(module, connection_type_methods) == 0) &&
        (PyModule_AddFunctions(module, decode_error_methods) == 0) &&
        (PyModule_AddFunctions(module, file_converter_status_methods) == 0) &&
        (PyModule_AddFunctions(module, result_methods) == 0) &&
        add_object(module, &command_message_object, "CommandMessage") &&
        add_object(module, &connection_object, "Connection") &&
        add_object(module, &usb_connection_info_object, "UsbConnectionInfo") &&
        add_object(module, &serial_connection_info_object, "SerialConnectionInfo") &&
        add_object(module, &tcp_connection_info_object, "TcpConnectionInfo") &&
        add_object(module, &udp_connection_info_object, "UdpConnectionInfo") &&
        add_object(module, &bluetooth_connection_info_object, "BluetoothConnectionInfo") &&
        add_object(module, &file_connection_info_object, "FileConnectionInfo") &&
        add_object(module, &data_logger_object, "DataLogger") &&
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
        add_object(module, &device_object, "Device") &&
        add_object(module, &statistics_object, "Statistics"))
    {
        return module;
    }

    Py_DECREF(module);

    return NULL;
}
