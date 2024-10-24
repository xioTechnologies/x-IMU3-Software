#pragma once

#include "../../C/Ximu3.h"
#include "Helpers.h"

using namespace System;

namespace Ximu3
{
    public ref class ConnectionInfo
    {
    };

    public ref class UsbConnectionInfo : public ConnectionInfo
    {
    internal:
        UsbConnectionInfo(ximu3::XIMU3_UsbConnectionInfo* connectionInfo) : connectionInfo{ new ximu3::XIMU3_UsbConnectionInfo{} }
        {
            *this->connectionInfo = *connectionInfo;
        }

    public:
        UsbConnectionInfo(String^ portName) : connectionInfo{ new ximu3::XIMU3_UsbConnectionInfo{} }
        {
            Helpers::StringCopy(connectionInfo->port_name, portName, sizeof(connectionInfo->port_name));
        }

        ~UsbConnectionInfo()
        {
            delete connectionInfo;
        }

        property String^ PortName
        {
            String^ get()
            {
                return gcnew String(connectionInfo->port_name);
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_usb_connection_info_to_string(*connectionInfo));
        }

    internal:
        ximu3::XIMU3_UsbConnectionInfo* connectionInfo;
    };

    public ref class SerialConnectionInfo : public ConnectionInfo
    {
    internal:
        SerialConnectionInfo(ximu3::XIMU3_SerialConnectionInfo* connectionInfo) : connectionInfo{ new ximu3::XIMU3_SerialConnectionInfo{} }
        {
            *this->connectionInfo = *connectionInfo;
        }

    public:
        SerialConnectionInfo(String^ portName, const uint32_t baudRate, const bool rtsCtsEnabled) : connectionInfo{ new ximu3::XIMU3_SerialConnectionInfo{} }
        {
            Helpers::StringCopy(connectionInfo->port_name, portName, sizeof(connectionInfo->port_name));
            connectionInfo->baud_rate = baudRate;
            connectionInfo->rts_cts_enabled = rtsCtsEnabled;
        }

        ~SerialConnectionInfo()
        {
            delete connectionInfo;
        }

        property String^ PortName
        {
            String^ get()
            {
                return gcnew String(connectionInfo->port_name);
            }
        }

        property uint32_t BaudRate
        {
            uint32_t get()
            {
                return connectionInfo->baud_rate;
            }
        }

        property bool RtsCtsEnabled
        {
            bool get()
            {
                return connectionInfo->rts_cts_enabled;
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_serial_connection_info_to_string(*connectionInfo));
        }

    internal:
        ximu3::XIMU3_SerialConnectionInfo* connectionInfo;
    };

    public ref class TcpConnectionInfo : public ConnectionInfo
    {
    internal:
        TcpConnectionInfo(ximu3::XIMU3_TcpConnectionInfo* connectionInfo) : connectionInfo{ new ximu3::XIMU3_TcpConnectionInfo{} }
        {
            *this->connectionInfo = *connectionInfo;
        }

    public:
        TcpConnectionInfo(String^ ipAddress, const uint16_t port) : connectionInfo{ new ximu3::XIMU3_TcpConnectionInfo{} }
        {
            Helpers::StringCopy(connectionInfo->ip_address, ipAddress, sizeof(connectionInfo->ip_address));
            connectionInfo->port = port;
        }

        ~TcpConnectionInfo()
        {
            delete connectionInfo;
        }

        property String^ IPAddress
        {
            String^ get()
            {
                return gcnew String(connectionInfo->ip_address);
            }
        }

        property uint16_t Port
        {
            uint16_t get()
            {
                return connectionInfo->port;
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_tcp_connection_info_to_string(*connectionInfo));
        }

    internal:
        ximu3::XIMU3_TcpConnectionInfo* connectionInfo;
    };

    public ref class UdpConnectionInfo : public ConnectionInfo
    {
    internal:
        UdpConnectionInfo(ximu3::XIMU3_UdpConnectionInfo* connectionInfo) : connectionInfo{ new ximu3::XIMU3_UdpConnectionInfo{} }
        {
            *this->connectionInfo = *connectionInfo;
        }

    public:
        UdpConnectionInfo(String^ ipAddress, const uint16_t sendPort, const uint16_t receivePort) : connectionInfo{ new ximu3::XIMU3_UdpConnectionInfo{} }
        {
            Helpers::StringCopy(connectionInfo->ip_address, ipAddress, sizeof(connectionInfo->ip_address));
            connectionInfo->send_port = sendPort;
            connectionInfo->receive_port = receivePort;
        }

        ~UdpConnectionInfo()
        {
            delete connectionInfo;
        }

        property String^ IPAddress
        {
            String^ get()
            {
                return gcnew String(connectionInfo->ip_address);
            }
        }

        property uint16_t SendPort
        {
            uint16_t get()
            {
                return connectionInfo->send_port;
            }
        }

        property uint16_t ReceivePort
        {
            uint16_t get()
            {
                return connectionInfo->receive_port;
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_udp_connection_info_to_string(*connectionInfo));
        }

    internal:
        ximu3::XIMU3_UdpConnectionInfo* connectionInfo;
    };

    public ref class BluetoothConnectionInfo : public ConnectionInfo
    {
    internal:
        BluetoothConnectionInfo(ximu3::XIMU3_BluetoothConnectionInfo* connectionInfo) : connectionInfo{ new ximu3::XIMU3_BluetoothConnectionInfo{} }
        {
            *this->connectionInfo = *connectionInfo;
        }

    public:
        BluetoothConnectionInfo(String^ portName) : connectionInfo{ new ximu3::XIMU3_BluetoothConnectionInfo{} }
        {
            Helpers::StringCopy(connectionInfo->port_name, portName, sizeof(connectionInfo->port_name));
        }

        ~BluetoothConnectionInfo()
        {
            delete connectionInfo;
        }

        property String^ PortName
        {
            String^ get()
            {
                return gcnew String(connectionInfo->port_name);
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_bluetooth_connection_info_to_string(*connectionInfo));
        }

    internal:
        ximu3::XIMU3_BluetoothConnectionInfo* connectionInfo;
    };

    public ref class FileConnectionInfo : public ConnectionInfo
    {
    internal:
        FileConnectionInfo(ximu3::XIMU3_FileConnectionInfo* connectionInfo) : connectionInfo{ new ximu3::XIMU3_FileConnectionInfo{} }
        {
            *this->connectionInfo = *connectionInfo;
        }

    public:
        FileConnectionInfo(String^ filePath) : connectionInfo{ new ximu3::XIMU3_FileConnectionInfo{} }
        {
            Helpers::StringCopy(connectionInfo->file_path, filePath, sizeof(connectionInfo->file_path));
        }

        ~FileConnectionInfo()
        {
            delete connectionInfo;
        }

        property String^ FilePath
        {
            String^ get()
            {
                return gcnew String(connectionInfo->file_path);
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_file_connection_info_to_string(*connectionInfo));
        }

    internal:
        ximu3::XIMU3_FileConnectionInfo* connectionInfo;
    };
}
