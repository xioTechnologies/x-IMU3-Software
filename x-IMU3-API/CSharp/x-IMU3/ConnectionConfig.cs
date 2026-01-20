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
            connectionConfig.port_name = Helpers.ToBytes(portName);
        }

        public UsbConnectionConfig(CApi.XIMU3_UsbConnectionConfig connectionConfig)
        {
            this.connectionConfig = connectionConfig;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_usb_connection_config_to_string(connectionConfig));
        }

        public CApi.XIMU3_UsbConnectionConfig connectionConfig = new();
    }

    public class SerialConnectionConfig : ConnectionConfig
    {
        public SerialConnectionConfig(string portName, UInt32 baudRate, bool rtsCtsEnabled)
        {
            connectionConfig.port_name = Helpers.ToBytes(portName);
            connectionConfig.baud_rate = baudRate;
            connectionConfig.rts_cts_enabled = rtsCtsEnabled;
        }

        public SerialConnectionConfig(CApi.XIMU3_SerialConnectionConfig connectionConfig)
        {
            this.connectionConfig = connectionConfig;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_serial_connection_config_to_string(connectionConfig));
        }

        public CApi.XIMU3_SerialConnectionConfig connectionConfig = new();
    }

    public class TcpConnectionConfig : ConnectionConfig
    {
        public TcpConnectionConfig(string ipAddress, UInt16 port)
        {
            connectionConfig.ip_address = Helpers.ToBytes(ipAddress);
            connectionConfig.port = port;
        }

        public TcpConnectionConfig(CApi.XIMU3_TcpConnectionConfig connectionConfig)
        {
            this.connectionConfig = connectionConfig;
        }

        public TcpConnectionConfig(CApi.XIMU3_NetworkAnnouncementMessage message)
        {
            this.connectionConfig = CApi.XIMU3_network_announcement_message_to_tcp_connection_config(message);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_tcp_connection_config_to_string(connectionConfig));
        }

        public CApi.XIMU3_TcpConnectionConfig connectionConfig = new();
    }

    public class UdpConnectionConfig : ConnectionConfig
    {
        public UdpConnectionConfig(string ipAddress, UInt16 sendPort, UInt16 receivePort)
        {
            connectionConfig.ip_address = Helpers.ToBytes(ipAddress);
            connectionConfig.send_port = sendPort;
            connectionConfig.receive_port = receivePort;
        }

        public UdpConnectionConfig(CApi.XIMU3_UdpConnectionConfig connectionConfig)
        {
            this.connectionConfig = connectionConfig;
        }

        public UdpConnectionConfig(CApi.XIMU3_NetworkAnnouncementMessage message)
        {
            this.connectionConfig = CApi.XIMU3_network_announcement_message_to_udp_connection_config(message);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_udp_connection_config_to_string(connectionConfig));
        }

        public CApi.XIMU3_UdpConnectionConfig connectionConfig = new();
    }

    public class BluetoothConnectionConfig : ConnectionConfig
    {
        public BluetoothConnectionConfig(string portName)
        {
            connectionConfig.port_name = Helpers.ToBytes(portName);
        }

        public BluetoothConnectionConfig(CApi.XIMU3_BluetoothConnectionConfig connectionConfig)
        {
            this.connectionConfig = connectionConfig;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_bluetooth_connection_config_to_string(connectionConfig));
        }

        public CApi.XIMU3_BluetoothConnectionConfig connectionConfig = new();
    }

    public class FileConnectionConfig : ConnectionConfig
    {
        public FileConnectionConfig(string filePath)
        {
            connectionConfig.file_path = Helpers.ToBytes(filePath);
        }

        public FileConnectionConfig(CApi.XIMU3_FileConnectionConfig connectionConfig)
        {
            this.connectionConfig = connectionConfig;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_file_connection_config_to_string(connectionConfig));
        }

        public CApi.XIMU3_FileConnectionConfig connectionConfig = new();
    }

    public class MuxConnectionConfig : ConnectionConfig, IDisposable
    {
        public MuxConnectionConfig(Byte channel, Connection connection)
        {
            connectionConfig = CApi.XIMU3_mux_connection_config_new(channel, connection.connection);
        }

        public MuxConnectionConfig(IntPtr connectionConfig)
        {
            this.connectionConfig = connectionConfig;
        }

        ~MuxConnectionConfig() => Dispose();

        public void Dispose()
        {
            if (connectionConfig != IntPtr.Zero)
            {
                CApi.XIMU3_mux_connection_config_free(connectionConfig);
                connectionConfig = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_mux_connection_config_to_string(connectionConfig));
        }

        public IntPtr connectionConfig;
    }
}
