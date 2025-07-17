using System;

namespace Ximu3
{
    public class ConnectionInfo
    {
        public static ConnectionInfo? From(CApi.XIMU3_Device device)
        {
            switch (device.connection_type)
            {
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUsb:
                    return new UsbConnectionInfo(device.usb_connection_info);
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeSerial:
                    return new SerialConnectionInfo(device.serial_connection_info);
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeBluetooth:
                    return new BluetoothConnectionInfo(device.bluetooth_connection_info);
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeTcp:
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUdp:
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeFile:
                case CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeMux:
                    break;
            }

            return null;
        }
    }

    public class UsbConnectionInfo : ConnectionInfo
    {
        public UsbConnectionInfo(string portName)
        {
            connectionInfo.port_name = Ximu3.Helpers.ToBytes(portName);
        }

        public UsbConnectionInfo(CApi.XIMU3_UsbConnectionInfo connectionInfo)
        {
            this.connectionInfo = connectionInfo;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_usb_connection_info_to_string(connectionInfo));
        }

        public CApi.XIMU3_UsbConnectionInfo connectionInfo = new();
    }

    public class SerialConnectionInfo : ConnectionInfo
    {
        public SerialConnectionInfo(string portName, UInt32 baudRate, bool rtsCtsEnabled)
        {
            connectionInfo.port_name = Ximu3.Helpers.ToBytes(portName);
            connectionInfo.baud_rate = baudRate;
            connectionInfo.rts_cts_enabled = rtsCtsEnabled;
        }

        public SerialConnectionInfo(CApi.XIMU3_SerialConnectionInfo connectionInfo)
        {
            this.connectionInfo = connectionInfo;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_serial_connection_info_to_string(connectionInfo));
        }

        public CApi.XIMU3_SerialConnectionInfo connectionInfo = new();
    }

    public class TcpConnectionInfo : ConnectionInfo
    {
        public TcpConnectionInfo(string ipAddress, UInt16 port)
        {
            connectionInfo.ip_address = Ximu3.Helpers.ToBytes(ipAddress);
            connectionInfo.port = port;
        }

        public TcpConnectionInfo(CApi.XIMU3_TcpConnectionInfo connectionInfo)
        {
            this.connectionInfo = connectionInfo;
        }

        public TcpConnectionInfo(CApi.XIMU3_NetworkAnnouncementMessage message)
        {
            this.connectionInfo = CApi.XIMU3_network_announcement_message_to_tcp_connection_info(message);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_tcp_connection_info_to_string(connectionInfo));
        }

        public CApi.XIMU3_TcpConnectionInfo connectionInfo = new();
    }

    public class UdpConnectionInfo : ConnectionInfo
    {
        public UdpConnectionInfo(string ipAddress, UInt16 sendPort, UInt16 receivePort)
        {
            connectionInfo.ip_address = Ximu3.Helpers.ToBytes(ipAddress);
            connectionInfo.send_port = sendPort;
            connectionInfo.receive_port = receivePort;
        }

        public UdpConnectionInfo(CApi.XIMU3_UdpConnectionInfo connectionInfo)
        {
            this.connectionInfo = connectionInfo;
        }

        public UdpConnectionInfo(CApi.XIMU3_NetworkAnnouncementMessage message)
        {
            this.connectionInfo = CApi.XIMU3_network_announcement_message_to_udp_connection_info(message);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_udp_connection_info_to_string(connectionInfo));
        }

        public CApi.XIMU3_UdpConnectionInfo connectionInfo = new();
    }

    public class BluetoothConnectionInfo : ConnectionInfo
    {
        public BluetoothConnectionInfo(string portName)
        {
            connectionInfo.port_name = Ximu3.Helpers.ToBytes(portName);
        }

        public BluetoothConnectionInfo(CApi.XIMU3_BluetoothConnectionInfo connectionInfo)
        {
            this.connectionInfo = connectionInfo;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_bluetooth_connection_info_to_string(connectionInfo));
        }

        public CApi.XIMU3_BluetoothConnectionInfo connectionInfo = new();
    }

    public class FileConnectionInfo : ConnectionInfo
    {
        public FileConnectionInfo(string filePath)
        {
            connectionInfo.file_path = Ximu3.Helpers.ToBytes(filePath);
        }

        public FileConnectionInfo(CApi.XIMU3_FileConnectionInfo connectionInfo)
        {
            this.connectionInfo = connectionInfo;
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_file_connection_info_to_string(connectionInfo));
        }

        public CApi.XIMU3_FileConnectionInfo connectionInfo = new();
    }

    public class MuxConnectionInfo : ConnectionInfo, IDisposable
    {
        public MuxConnectionInfo(Byte channel, Connection connection)
        {
            connectionInfo = CApi.XIMU3_mux_connection_info_new(channel, connection.connection);
        }

        public MuxConnectionInfo(IntPtr connectionInfo)
        {
            this.connectionInfo = connectionInfo;
        }

        ~MuxConnectionInfo() => Dispose();

        public void Dispose()
        {
            if (connectionInfo != IntPtr.Zero)
            {
                CApi.XIMU3_mux_connection_info_free(connectionInfo);
                connectionInfo = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        public override string ToString()
        {
            return Helpers.ToString(CApi.XIMU3_mux_connection_info_to_string(connectionInfo));
        }

        public IntPtr connectionInfo;
    }
}
