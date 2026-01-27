using System;

namespace Ximu3
{
    public class ConnectionConfig
    {
        public static ConnectionConfig? From(CApi.XIMU3_Device device)
        {
            switch (device.connection_type)
            {
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUsb:
                    return new UsbConnectionConfig(device.usb_connection_config);
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeSerial:
                    return new SerialConnectionConfig(device.serial_connection_config);
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeBluetooth:
                    return new BluetoothConnectionConfig(device.bluetooth_connection_config);
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeTcp:
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUdp:
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeFile:
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeMux:
                    break;
            }

            return null;
        }
    }

    public class UsbConnectionConfig : ConnectionConfig
    {
        public UsbConnectionConfig(string portName)
        {
            wrapped.port_name = Helpers.ToBytes(portName);
        }

        public UsbConnectionConfig(CApi.XIMU3_UsbConnectionConfig config)
        {
            this.wrapped = config;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_usb_connection_config_to_string(wrapped));
        }

        public CApi.XIMU3_UsbConnectionConfig wrapped = new();
    }

    public class SerialConnectionConfig : ConnectionConfig
    {
        public SerialConnectionConfig(string portName, UInt32 baudRate, bool rtsCtsEnabled)
        {
            wrapped.port_name = Helpers.ToBytes(portName);
            wrapped.baud_rate = baudRate;
            wrapped.rts_cts_enabled = rtsCtsEnabled;
        }

        public SerialConnectionConfig(CApi.XIMU3_SerialConnectionConfig config)
        {
            this.wrapped = config;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_serial_connection_config_to_string(wrapped));
        }

        public CApi.XIMU3_SerialConnectionConfig wrapped = new();
    }

    public class TcpConnectionConfig : ConnectionConfig
    {
        public TcpConnectionConfig(string ipAddress, UInt16 port)
        {
            wrapped.ip_address = Helpers.ToBytes(ipAddress);
            wrapped.port = port;
        }

        public TcpConnectionConfig(CApi.XIMU3_TcpConnectionConfig config)
        {
            this.wrapped = config;
        }

        public TcpConnectionConfig(CApi.XIMU3_NetworkAnnouncementMessage message)
        {
            this.wrapped = CApi.XIMU3_network_announcement_message_to_tcp_connection_config(message);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_tcp_connection_config_to_string(wrapped));
        }

        public CApi.XIMU3_TcpConnectionConfig wrapped = new();
    }

    public class UdpConnectionConfig : ConnectionConfig
    {
        public UdpConnectionConfig(string ipAddress, UInt16 sendPort, UInt16 receivePort)
        {
            wrapped.ip_address = Helpers.ToBytes(ipAddress);
            wrapped.send_port = sendPort;
            wrapped.receive_port = receivePort;
        }

        public UdpConnectionConfig(CApi.XIMU3_UdpConnectionConfig config)
        {
            this.wrapped = config;
        }

        public UdpConnectionConfig(CApi.XIMU3_NetworkAnnouncementMessage message)
        {
            this.wrapped = CApi.XIMU3_network_announcement_message_to_udp_connection_config(message);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_udp_connection_config_to_string(wrapped));
        }

        public CApi.XIMU3_UdpConnectionConfig wrapped = new();
    }

    public class BluetoothConnectionConfig : ConnectionConfig
    {
        public BluetoothConnectionConfig(string portName)
        {
            wrapped.port_name = Helpers.ToBytes(portName);
        }

        public BluetoothConnectionConfig(CApi.XIMU3_BluetoothConnectionConfig config)
        {
            this.wrapped = config;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_bluetooth_connection_config_to_string(wrapped));
        }

        public CApi.XIMU3_BluetoothConnectionConfig wrapped = new();
    }

    public class FileConnectionConfig : ConnectionConfig
    {
        public FileConnectionConfig(string filePath)
        {
            wrapped.file_path = Helpers.ToBytes(filePath);
        }

        public FileConnectionConfig(CApi.XIMU3_FileConnectionConfig config)
        {
            this.wrapped = config;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_file_connection_config_to_string(wrapped));
        }

        public CApi.XIMU3_FileConnectionConfig wrapped = new();
    }

    public class MuxConnectionConfig : ConnectionConfig, IDisposable
    {
        public MuxConnectionConfig(Byte channel, Connection connection)
        {
            wrapped = CApi.XIMU3_mux_connection_config_new(channel, connection.wrapped);
        }

        public MuxConnectionConfig(IntPtr config)
        {
            this.wrapped = config;
        }

        ~MuxConnectionConfig() => Dispose();

        public void Dispose()
        {
            if (wrapped != IntPtr.Zero)
            {
                CApi.XIMU3_mux_connection_config_free(wrapped);
                wrapped = IntPtr.Zero;
            }

            GC.SuppressFinalize(this);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_mux_connection_config_to_string(wrapped));
        }

        public IntPtr wrapped;
    }
}
