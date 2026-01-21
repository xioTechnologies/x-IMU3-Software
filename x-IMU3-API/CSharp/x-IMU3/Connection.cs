using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class Connection : IDisposable
    {
        public Connection(ConnectionConfig config)
        {
            switch (config)
            {
                case UsbConnectionConfig usbConnectionConfig:
                    connection = CApi.XIMU3_connection_new_usb(usbConnectionConfig.connectionConfig);
                    return;
                case SerialConnectionConfig serialConnectionConfig:
                    connection = CApi.XIMU3_connection_new_serial(serialConnectionConfig.connectionConfig);
                    return;
                case TcpConnectionConfig tcpConnectionConfig:
                    connection = CApi.XIMU3_connection_new_tcp(tcpConnectionConfig.connectionConfig);
                    return;
                case UdpConnectionConfig udpConnectionConfig:
                    connection = CApi.XIMU3_connection_new_udp(udpConnectionConfig.connectionConfig);
                    return;
                case BluetoothConnectionConfig bluetoothConnectionConfig:
                    connection = CApi.XIMU3_connection_new_bluetooth(bluetoothConnectionConfig.connectionConfig);
                    return;
                case FileConnectionConfig fileConnectionConfig:
                    connection = CApi.XIMU3_connection_new_file(fileConnectionConfig.connectionConfig);
                    return;
                case MuxConnectionConfig muxConnectionConfig:
                    connection = CApi.XIMU3_connection_new_mux(muxConnectionConfig.connectionConfig);
                    return;
            }

            Debug.Assert(false);
        }

        ~Connection() => Dispose();

        public void Dispose()
        {
            if (connection != IntPtr.Zero)
            {
                CApi.XIMU3_connection_free(connection);

                connection = IntPtr.Zero;
            }

            GC.SuppressFinalize(this);
        }

        public CApi.XIMU3_Result Open()
        {
            return CApi.XIMU3_connection_open(connection);
        }

        public delegate void OpenAsyncCallback(CApi.XIMU3_Result result);

        private static void OpenAsyncCallbackInternal(CApi.XIMU3_Result result, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<OpenAsyncCallback>(context)(result);
        }

        public void OpenAsync(OpenAsyncCallback callback)
        {
            CApi.XIMU3_connection_open_async(connection, OpenAsyncCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public void Close()
        {
            CApi.XIMU3_connection_close(connection);
        }

        public CApi.XIMU3_PingResponse Ping()
        {
            return CApi.XIMU3_connection_ping(connection);
        }

        public delegate void PingAsyncCallback(CApi.XIMU3_PingResponse? response);

        private static void PingAsyncCallbackInternal(CApi.XIMU3_PingResponse response, IntPtr context)
        {
            if (Helpers.ToString(response.interface_).Length == 0)
            {
                Marshal.GetDelegateForFunctionPointer<PingAsyncCallback>(context)(null);
                return;
            }

            Marshal.GetDelegateForFunctionPointer<PingAsyncCallback>(context)(response);
        }

        public void PingAsync(PingAsyncCallback callback)
        {
            CApi.XIMU3_connection_ping_async(connection, PingAsyncCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public CommandMessage? SendCommand(string command, UInt32 retries = CApi.XIMU3_DEFAULT_RETRIES, UInt32 timeout = CApi.XIMU3_DEFAULT_TIMEOUT)
        {
            return CommandMessage.From(CApi.XIMU3_connection_send_command(connection, Helpers.ToPointer(command), retries, timeout));
        }

        public CommandMessage?[] SendCommands(string[] commands, UInt32 retries = CApi.XIMU3_DEFAULT_RETRIES, UInt32 timeout = CApi.XIMU3_DEFAULT_TIMEOUT)
        {
            return ToArrayAndFree(CApi.XIMU3_connection_send_commands(connection, Marshal.UnsafeAddrOfPinnedArrayElement(Helpers.ToPointers(commands), 0), (UInt32)commands.Length, retries, timeout));
        }

        public delegate void SendCommandAsyncCallback(CommandMessage? responses);

        private static void SendCommandAsyncCallbackInternal(CApi.XIMU3_CommandMessage data, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<SendCommandAsyncCallback>(context)(CommandMessage.From(data));
        }

        public void SendCommandAsync(string command, SendCommandsAsyncCallback callback, UInt32 retries = CApi.XIMU3_DEFAULT_RETRIES, UInt32 timeout = CApi.XIMU3_DEFAULT_TIMEOUT)
        {
            CApi.XIMU3_connection_send_command_async(connection, Helpers.ToPointer(command), retries, timeout, SendCommandAsyncCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void SendCommandsAsyncCallback(CommandMessage?[] responses);

        private static void SendCommandsAsyncCallbackInternal(CApi.XIMU3_CommandMessages data, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<SendCommandsAsyncCallback>(context)(ToArrayAndFree(data));
        }

        public void SendCommandsAsync(string[] commands, SendCommandsAsyncCallback callback, UInt32 retries = CApi.XIMU3_DEFAULT_RETRIES, UInt32 timeout = CApi.XIMU3_DEFAULT_TIMEOUT)
        {
            CApi.XIMU3_connection_send_commands_async(connection, Marshal.UnsafeAddrOfPinnedArrayElement(Helpers.ToPointers(commands), 0), (UInt32)commands.Length, retries, timeout, SendCommandsAsyncCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public object? GetConfig()
        {
            return CApi.XIMU3_connection_get_type(connection) switch
            {
                CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUsb => new UsbConnectionConfig(CApi.XIMU3_connection_get_config_usb(connection)),
                CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeSerial => new SerialConnectionConfig(CApi.XIMU3_connection_get_config_serial(connection)),
                CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeTcp => new TcpConnectionConfig(CApi.XIMU3_connection_get_config_tcp(connection)),
                CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUdp => new UdpConnectionConfig(CApi.XIMU3_connection_get_config_udp(connection)),
                CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeBluetooth => new BluetoothConnectionConfig(CApi.XIMU3_connection_get_config_bluetooth(connection)),
                CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeFile => new FileConnectionConfig(CApi.XIMU3_connection_get_config_file(connection)),
                CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeMux => new MuxConnectionConfig(CApi.XIMU3_connection_get_config_mux(connection)),
                _ => null,
            };
        }

        public CApi.XIMU3_Statistics GetStatistics()
        {
            return CApi.XIMU3_connection_get_statistics(connection);
        }

        public delegate void ReceiveErrorCallback(CApi.XIMU3_ReceiveError receiveError);

        private static void ReceiveErrorCallbackInternal(CApi.XIMU3_ReceiveError receiveError, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<ReceiveErrorCallback>(context)(receiveError);
        }

        public UInt64 AddReceiveErrorCallback(ReceiveErrorCallback callback)
        {
            return CApi.XIMU3_connection_add_receive_error_callback(connection, ReceiveErrorCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void StatisticsCallback(CApi.XIMU3_Statistics statistics);

        private static void StatisticsCallbackInternal(CApi.XIMU3_Statistics statistics, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<StatisticsCallback>(context)(statistics);
        }

        public UInt64 AddStatisticsCallback(StatisticsCallback callback)
        {
            return CApi.XIMU3_connection_add_statistics_callback(connection, StatisticsCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        // Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
        public delegate void InertialCallback(CApi.XIMU3_InertialMessage message);

        private static void InertialCallbackInternal(CApi.XIMU3_InertialMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<InertialCallback>(context)(message);
        }

        public UInt64 AddInertialCallback(InertialCallback callback)
        {
            return CApi.XIMU3_connection_add_inertial_callback(connection, InertialCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void MagnetometerCallback(CApi.XIMU3_MagnetometerMessage message);

        private static void MagnetometerCallbackInternal(CApi.XIMU3_MagnetometerMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<MagnetometerCallback>(context)(message);
        }

        public UInt64 AddMagnetometerCallback(MagnetometerCallback callback)
        {
            return CApi.XIMU3_connection_add_magnetometer_callback(connection, MagnetometerCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void QuaternionCallback(CApi.XIMU3_QuaternionMessage message);

        private static void QuaternionCallbackInternal(CApi.XIMU3_QuaternionMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<QuaternionCallback>(context)(message);
        }

        public UInt64 AddQuaternionCallback(QuaternionCallback callback)
        {
            return CApi.XIMU3_connection_add_quaternion_callback(connection, QuaternionCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void RotationMatrixCallback(CApi.XIMU3_RotationMatrixMessage message);

        private static void RotationMatrixCallbackInternal(CApi.XIMU3_RotationMatrixMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<RotationMatrixCallback>(context)(message);
        }

        public UInt64 AddRotationMatrixCallback(RotationMatrixCallback callback)
        {
            return CApi.XIMU3_connection_add_rotation_matrix_callback(connection, RotationMatrixCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void EulerAnglesCallback(CApi.XIMU3_EulerAnglesMessage message);

        private static void EulerAnglesCallbackInternal(CApi.XIMU3_EulerAnglesMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<EulerAnglesCallback>(context)(message);
        }

        public UInt64 AddEulerAnglesCallback(EulerAnglesCallback callback)
        {
            return CApi.XIMU3_connection_add_euler_angles_callback(connection, EulerAnglesCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void LinearAccelerationCallback(CApi.XIMU3_LinearAccelerationMessage message);

        private static void LinearAccelerationCallbackInternal(CApi.XIMU3_LinearAccelerationMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<LinearAccelerationCallback>(context)(message);
        }

        public UInt64 AddLinearAccelerationCallback(LinearAccelerationCallback callback)
        {
            return CApi.XIMU3_connection_add_linear_acceleration_callback(connection, LinearAccelerationCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void EarthAccelerationCallback(CApi.XIMU3_EarthAccelerationMessage message);

        private static void EarthAccelerationCallbackInternal(CApi.XIMU3_EarthAccelerationMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<EarthAccelerationCallback>(context)(message);
        }

        public UInt64 AddEarthAccelerationCallback(EarthAccelerationCallback callback)
        {
            return CApi.XIMU3_connection_add_earth_acceleration_callback(connection, EarthAccelerationCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void AhrsStatusCallback(CApi.XIMU3_AhrsStatusMessage message);

        private static void AhrsStatusCallbackInternal(CApi.XIMU3_AhrsStatusMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<AhrsStatusCallback>(context)(message);
        }

        public UInt64 AddAhrsStatusCallback(AhrsStatusCallback callback)
        {
            return CApi.XIMU3_connection_add_ahrs_status_callback(connection, AhrsStatusCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void HighGAccelerometerCallback(CApi.XIMU3_HighGAccelerometerMessage message);

        private static void HighGAccelerometerCallbackInternal(CApi.XIMU3_HighGAccelerometerMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<HighGAccelerometerCallback>(context)(message);
        }

        public UInt64 AddHighGAccelerometerCallback(HighGAccelerometerCallback callback)
        {
            return CApi.XIMU3_connection_add_high_g_accelerometer_callback(connection, HighGAccelerometerCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void TemperatureCallback(CApi.XIMU3_TemperatureMessage message);

        private static void TemperatureCallbackInternal(CApi.XIMU3_TemperatureMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<TemperatureCallback>(context)(message);
        }

        public UInt64 AddTemperatureCallback(TemperatureCallback callback)
        {
            return CApi.XIMU3_connection_add_temperature_callback(connection, TemperatureCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void BatteryCallback(CApi.XIMU3_BatteryMessage message);

        private static void BatteryCallbackInternal(CApi.XIMU3_BatteryMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<BatteryCallback>(context)(message);
        }

        public UInt64 AddBatteryCallback(BatteryCallback callback)
        {
            return CApi.XIMU3_connection_add_battery_callback(connection, BatteryCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void RssiCallback(CApi.XIMU3_RssiMessage message);

        private static void RssiCallbackInternal(CApi.XIMU3_RssiMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<RssiCallback>(context)(message);
        }

        public UInt64 AddRssiCallback(RssiCallback callback)
        {
            return CApi.XIMU3_connection_add_rssi_callback(connection, RssiCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void SerialAccessoryCallback(CApi.XIMU3_SerialAccessoryMessage message);

        private static void SerialAccessoryCallbackInternal(CApi.XIMU3_SerialAccessoryMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<SerialAccessoryCallback>(context)(message);
        }

        public UInt64 AddSerialAccessoryCallback(SerialAccessoryCallback callback)
        {
            return CApi.XIMU3_connection_add_serial_accessory_callback(connection, SerialAccessoryCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void NotificationCallback(CApi.XIMU3_NotificationMessage message);

        private static void NotificationCallbackInternal(CApi.XIMU3_NotificationMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<NotificationCallback>(context)(message);
        }

        public UInt64 AddNotificationCallback(NotificationCallback callback)
        {
            return CApi.XIMU3_connection_add_notification_callback(connection, NotificationCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void ErrorCallback(CApi.XIMU3_ErrorMessage message);

        private static void ErrorCallbackInternal(CApi.XIMU3_ErrorMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<ErrorCallback>(context)(message);
        }

        public UInt64 AddErrorCallback(ErrorCallback callback)
        {
            return CApi.XIMU3_connection_add_error_callback(connection, ErrorCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        // End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

        public delegate void EndOfFileCallback();

        private static void EndOfFileCallbackInternal(IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<EndOfFileCallback>(context)();
        }

        public UInt64 AddEndOfFileCallback(EndOfFileCallback callback)
        {
            return CApi.XIMU3_connection_add_end_of_file_callback(connection, EndOfFileCallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public void RemoveCallback(UInt64 callbackId)
        {
            CApi.XIMU3_connection_remove_callback(connection, callbackId);
        }

        private static CommandMessage?[] ToArrayAndFree(CApi.XIMU3_CommandMessages responses_)
        {
            var array = new CommandMessage?[responses_.length];

            for (var i = 0; i < responses_.length; i++)
            {
                array[i] = CommandMessage.From(Marshal.PtrToStructure<CApi.XIMU3_CommandMessage>(responses_.array + i * Marshal.SizeOf(typeof(CApi.XIMU3_CommandMessage))));
            }

            CApi.XIMU3_command_messages_free(responses_);

            return array;
        }

        internal IntPtr connection;
    }
}
