using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public static class CApi
    {
        public const int XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE = 256;
        public const int XIMU3_CHAR_ARRAY_SIZE = 256;

        public enum XIMU3_ChargingStatus
        {
            XIMU3_ChargingStatusNotConnected,
            XIMU3_ChargingStatusCharging,
            XIMU3_ChargingStatusChargingComplete,
            XIMU3_ChargingStatusChargingOnHold,
        }
        public enum XIMU3_ConnectionType
        {
            XIMU3_ConnectionTypeUsb,
            XIMU3_ConnectionTypeSerial,
            XIMU3_ConnectionTypeTcp,
            XIMU3_ConnectionTypeUdp,
            XIMU3_ConnectionTypeBluetooth,
            XIMU3_ConnectionTypeFile,
        }
        public enum XIMU3_DecodeError
        {
            XIMU3_DecodeErrorBufferOverrun,
            XIMU3_DecodeErrorInvalidMessageIdentifier,
            XIMU3_DecodeErrorInvalidUtf8,
            XIMU3_DecodeErrorInvalidJson,
            XIMU3_DecodeErrorJsonIsNotAnObject,
            XIMU3_DecodeErrorJsonObjectIsNotASingleKeyValuePair,
            XIMU3_DecodeErrorInvalidEscapeSequence,
            XIMU3_DecodeErrorInvalidBinaryMessageLength,
            XIMU3_DecodeErrorUnableToParseAsciiMessage,
        }
        public enum XIMU3_FileConverterStatus
        {
            XIMU3_FileConverterStatusComplete,
            XIMU3_FileConverterStatusFailed,
            XIMU3_FileConverterStatusInProgress,
        }
        public enum XIMU3_Result
        {
            XIMU3_ResultOk,
            XIMU3_ResultError,
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_CharArrays
        {
            public IntPtr array;
            public UInt32 length;
            public UInt32 capacity;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_CommandMessage
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] json;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] key;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] value;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] error;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_UsbConnectionInfo
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] port_name;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_SerialConnectionInfo
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] port_name;
            public UInt32 baud_rate;
            public bool rts_cts_enabled;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_TcpConnectionInfo
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] ip_address;
            public UInt16 port;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_UdpConnectionInfo
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] ip_address;
            public UInt16 send_port;
            public UInt16 receive_port;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_BluetoothConnectionInfo
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] port_name;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_FileConnectionInfo
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] file_path;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_PingResponse
        {
            public XIMU3_Result result;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] interface_;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] device_name;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] serial_number;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_Statistics
        {
            public UInt64 timestamp;
            public UInt64 data_total;
            public UInt32 data_rate;
            public UInt64 message_total;
            public UInt32 message_rate;
            public UInt64 error_total;
            public UInt32 error_rate;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_InertialMessage
        {
            public UInt64 timestamp;
            public float gyroscope_x;
            public float gyroscope_y;
            public float gyroscope_z;
            public float accelerometer_x;
            public float accelerometer_y;
            public float accelerometer_z;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_MagnetometerMessage
        {
            public UInt64 timestamp;
            public float x;
            public float y;
            public float z;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_QuaternionMessage
        {
            public UInt64 timestamp;
            public float w;
            public float x;
            public float y;
            public float z;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_RotationMatrixMessage
        {
            public UInt64 timestamp;
            public float xx;
            public float xy;
            public float xz;
            public float yx;
            public float yy;
            public float yz;
            public float zx;
            public float zy;
            public float zz;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_EulerAnglesMessage
        {
            public UInt64 timestamp;
            public float roll;
            public float pitch;
            public float yaw;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_LinearAccelerationMessage
        {
            public UInt64 timestamp;
            public float quaternion_w;
            public float quaternion_x;
            public float quaternion_y;
            public float quaternion_z;
            public float acceleration_x;
            public float acceleration_y;
            public float acceleration_z;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_EarthAccelerationMessage
        {
            public UInt64 timestamp;
            public float quaternion_w;
            public float quaternion_x;
            public float quaternion_y;
            public float quaternion_z;
            public float acceleration_x;
            public float acceleration_y;
            public float acceleration_z;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_AhrsStatusMessage
        {
            public UInt64 timestamp;
            public float initialising;
            public float angular_rate_recovery;
            public float acceleration_recovery;
            public float magnetic_recovery;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_HighGAccelerometerMessage
        {
            public UInt64 timestamp;
            public float x;
            public float y;
            public float z;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_TemperatureMessage
        {
            public UInt64 timestamp;
            public float temperature;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_BatteryMessage
        {
            public UInt64 timestamp;
            public float percentage;
            public float voltage;
            public float charging_status;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_RssiMessage
        {
            public UInt64 timestamp;
            public float percentage;
            public float power;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_SerialAccessoryMessage
        {
            public UInt64 timestamp;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE)]
            public byte[] char_array;
            public UIntPtr number_of_bytes;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_NotificationMessage
        {
            public UInt64 timestamp;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE)]
            public byte[] char_array;
            public UIntPtr number_of_bytes;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_ErrorMessage
        {
            public UInt64 timestamp;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE)]
            public byte[] char_array;
            public UIntPtr number_of_bytes;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_FileConverterProgress
        {
            public XIMU3_FileConverterStatus status;
            public float percentage;
            public UInt64 bytes_processed;
            public UInt64 bytes_total;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_NetworkAnnouncementMessage
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] device_name;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] serial_number;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] ip_address;
            public UInt16 tcp_port;
            public UInt16 udp_send;
            public UInt16 udp_receive;
            public Int32 rssi;
            public Int32 battery;
            public XIMU3_ChargingStatus charging_status;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_NetworkAnnouncementMessages
        {
            public IntPtr array;
            public UInt32 length;
            public UInt32 capacity;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_Device
        {
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] device_name;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
            public byte[] serial_number;
            public XIMU3_ConnectionType connection_type;
            public XIMU3_UsbConnectionInfo usb_connection_info;
            public XIMU3_SerialConnectionInfo serial_connection_info;
            public XIMU3_BluetoothConnectionInfo bluetooth_connection_info;
        }
        [StructLayout(LayoutKind.Sequential)]
        public struct XIMU3_Devices
        {
            public IntPtr array;
            public UInt32 length;
            public UInt32 capacity;
        }
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackResult(XIMU3_Result data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackPingResponseC(XIMU3_PingResponse data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackCharArrays(XIMU3_CharArrays data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackDecodeError(XIMU3_DecodeError data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackStatistics(XIMU3_Statistics data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackInertialMessage(XIMU3_InertialMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackMagnetometerMessage(XIMU3_MagnetometerMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackQuaternionMessage(XIMU3_QuaternionMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackRotationMatrixMessage(XIMU3_RotationMatrixMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackEulerAnglesMessage(XIMU3_EulerAnglesMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackLinearAccelerationMessage(XIMU3_LinearAccelerationMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackEarthAccelerationMessage(XIMU3_EarthAccelerationMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackAhrsStatusMessage(XIMU3_AhrsStatusMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackHighGAccelerometerMessage(XIMU3_HighGAccelerometerMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackTemperatureMessage(XIMU3_TemperatureMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackBatteryMessage(XIMU3_BatteryMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackRssiMessage(XIMU3_RssiMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackSerialAccessoryMessage(XIMU3_SerialAccessoryMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackNotificationMessage(XIMU3_NotificationMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackErrorMessage(XIMU3_ErrorMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackFileConverterProgress(XIMU3_FileConverterProgress data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackNetworkAnnouncementMessageC(XIMU3_NetworkAnnouncementMessage data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackDevices(XIMU3_Devices data, IntPtr context);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void XIMU3_CallbackEndOfFile(IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_char_arrays_free(XIMU3_CharArrays char_arrays);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_ChargingStatus XIMU3_charging_status_from_float(float charging_status);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_charging_status_to_string(XIMU3_ChargingStatus charging_status);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_CommandMessage XIMU3_command_message_parse(IntPtr json);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_connection_new_usb(XIMU3_UsbConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_connection_new_serial(XIMU3_SerialConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_connection_new_tcp(XIMU3_TcpConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_connection_new_udp(XIMU3_UdpConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_connection_new_bluetooth(XIMU3_BluetoothConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_connection_new_file(XIMU3_FileConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_connection_free(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_Result XIMU3_connection_open(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_connection_open_async(IntPtr connection, XIMU3_CallbackResult callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_connection_close(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_PingResponse XIMU3_connection_ping(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_connection_ping_async(IntPtr connection, XIMU3_CallbackPingResponseC callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_CharArrays XIMU3_connection_send_commands(IntPtr connection, IntPtr commands, UInt32 length, UInt32 retries, UInt32 timeout);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_connection_send_commands_async(IntPtr connection, IntPtr commands, UInt32 length, UInt32 retries, UInt32 timeout, XIMU3_CallbackCharArrays callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_ConnectionType XIMU3_connection_get_type(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_UsbConnectionInfo XIMU3_connection_get_info_usb(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_SerialConnectionInfo XIMU3_connection_get_info_serial(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_TcpConnectionInfo XIMU3_connection_get_info_tcp(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_UdpConnectionInfo XIMU3_connection_get_info_udp(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_BluetoothConnectionInfo XIMU3_connection_get_info_bluetooth(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_FileConnectionInfo XIMU3_connection_get_info_file(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_Statistics XIMU3_connection_get_statistics(IntPtr connection);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_decode_error_callback(IntPtr connection, XIMU3_CallbackDecodeError callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_statistics_callback(IntPtr connection, XIMU3_CallbackStatistics callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_inertial_callback(IntPtr connection, XIMU3_CallbackInertialMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_magnetometer_callback(IntPtr connection, XIMU3_CallbackMagnetometerMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_quaternion_callback(IntPtr connection, XIMU3_CallbackQuaternionMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_rotation_matrix_callback(IntPtr connection, XIMU3_CallbackRotationMatrixMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_euler_angles_callback(IntPtr connection, XIMU3_CallbackEulerAnglesMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_linear_acceleration_callback(IntPtr connection, XIMU3_CallbackLinearAccelerationMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_earth_acceleration_callback(IntPtr connection, XIMU3_CallbackEarthAccelerationMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_ahrs_status_callback(IntPtr connection, XIMU3_CallbackAhrsStatusMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_high_g_accelerometer_callback(IntPtr connection, XIMU3_CallbackHighGAccelerometerMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_temperature_callback(IntPtr connection, XIMU3_CallbackTemperatureMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_battery_callback(IntPtr connection, XIMU3_CallbackBatteryMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_rssi_callback(IntPtr connection, XIMU3_CallbackRssiMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_serial_accessory_callback(IntPtr connection, XIMU3_CallbackSerialAccessoryMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_notification_callback(IntPtr connection, XIMU3_CallbackNotificationMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_error_callback(IntPtr connection, XIMU3_CallbackErrorMessage callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_connection_add_end_of_file_callback(IntPtr connection, XIMU3_CallbackEndOfFile callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_connection_remove_callback(IntPtr connection, UInt64 callback_id);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_usb_connection_info_to_string(XIMU3_UsbConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_serial_connection_info_to_string(XIMU3_SerialConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_tcp_connection_info_to_string(XIMU3_TcpConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_udp_connection_info_to_string(XIMU3_UdpConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_bluetooth_connection_info_to_string(XIMU3_BluetoothConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_file_connection_info_to_string(XIMU3_FileConnectionInfo connection_info);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_connection_type_to_string(XIMU3_ConnectionType connection_type);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_data_logger_new(IntPtr destination, IntPtr name, IntPtr connections, UInt32 length);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_data_logger_free(IntPtr data_logger);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_Result XIMU3_data_logger_get_result(IntPtr data_logger);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_Result XIMU3_data_logger_log(IntPtr destination, IntPtr name, IntPtr connections, UInt32 length, UInt32 seconds);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_inertial_message_to_string(XIMU3_InertialMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_magnetometer_message_to_string(XIMU3_MagnetometerMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_quaternion_message_to_string(XIMU3_QuaternionMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_rotation_matrix_message_to_string(XIMU3_RotationMatrixMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_euler_angles_message_to_string(XIMU3_EulerAnglesMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_linear_acceleration_message_to_string(XIMU3_LinearAccelerationMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_earth_acceleration_message_to_string(XIMU3_EarthAccelerationMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_ahrs_status_message_to_string(XIMU3_AhrsStatusMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_high_g_accelerometer_message_to_string(XIMU3_HighGAccelerometerMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_temperature_message_to_string(XIMU3_TemperatureMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_battery_message_to_string(XIMU3_BatteryMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_rssi_message_to_string(XIMU3_RssiMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_serial_accessory_message_to_string(XIMU3_SerialAccessoryMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_notification_message_to_string(XIMU3_NotificationMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_error_message_to_string(XIMU3_ErrorMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_EulerAnglesMessage XIMU3_quaternion_message_to_euler_angles_message(XIMU3_QuaternionMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_EulerAnglesMessage XIMU3_rotation_matrix_message_to_euler_angles_message(XIMU3_RotationMatrixMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_QuaternionMessage XIMU3_euler_angles_message_to_quaternion_message(XIMU3_EulerAnglesMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_EulerAnglesMessage XIMU3_linear_acceleration_message_to_euler_angles_message(XIMU3_LinearAccelerationMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_EulerAnglesMessage XIMU3_earth_acceleration_message_to_euler_angles_message(XIMU3_EarthAccelerationMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_decode_error_to_string(XIMU3_DecodeError decode_error);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_file_converter_status_to_string(XIMU3_FileConverterStatus status);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_file_converter_progress_to_string(XIMU3_FileConverterProgress progress);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_file_converter_new(IntPtr destination, IntPtr name, IntPtr file_paths, UInt32 length, XIMU3_CallbackFileConverterProgress callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_file_converter_free(IntPtr file_converter);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_FileConverterProgress XIMU3_file_converter_convert(IntPtr destination, IntPtr name, IntPtr file_paths, UInt32 length);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_TcpConnectionInfo XIMU3_network_announcement_message_to_tcp_connection_info(XIMU3_NetworkAnnouncementMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_UdpConnectionInfo XIMU3_network_announcement_message_to_udp_connection_info(XIMU3_NetworkAnnouncementMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_network_announcement_message_to_string(XIMU3_NetworkAnnouncementMessage message);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_network_announcement_messages_free(XIMU3_NetworkAnnouncementMessages messages);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_network_announcement_new();
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_network_announcement_free(IntPtr network_announcement);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_Result XIMU3_network_announcement_get_result(IntPtr network_announcement);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt64 XIMU3_network_announcement_add_callback(IntPtr network_announcement, XIMU3_CallbackNetworkAnnouncementMessageC callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_network_announcement_remove_callback(IntPtr network_announcement, UInt64 callback_id);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_NetworkAnnouncementMessages XIMU3_network_announcement_get_messages(IntPtr network_announcement);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_NetworkAnnouncementMessages XIMU3_network_announcement_get_messages_after_short_delay(IntPtr network_announcement);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_ping_response_to_string(XIMU3_PingResponse ping_response);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_device_to_string(XIMU3_Device device);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_devices_free(XIMU3_Devices devices);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_port_scanner_new(XIMU3_CallbackDevices callback, IntPtr context);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern void XIMU3_port_scanner_free(IntPtr port_scanner);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_Devices XIMU3_port_scanner_get_devices(IntPtr port_scanner);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_Devices XIMU3_port_scanner_scan();
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_Devices XIMU3_port_scanner_scan_filter(XIMU3_ConnectionType connection_type);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern XIMU3_CharArrays XIMU3_port_scanner_get_port_names();
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_result_to_string(XIMU3_Result result);
        [DllImport("ximu3", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr XIMU3_statistics_to_string(XIMU3_Statistics statistics);
    }
}