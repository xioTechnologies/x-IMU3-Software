#pragma once

#include "../C/Ximu3.h"
#include <cstring>
#include <string>

namespace ximu3
{
    class ConnectionInfo
    {
    public:
        ConnectionInfo() = default;

        ConnectionInfo(const ConnectionInfo&) = delete;

        ConnectionInfo& operator=(const ConnectionInfo&) = delete;

        virtual ~ConnectionInfo() = default;

        virtual std::string toString() const = 0;

    protected:
        static void stringCopy(char* destination, const char* source, const size_t destinationSize)
        {
            strncpy(destination, source, destinationSize);
            destination[destinationSize - 1] = '\0';
        }
    };

    class UsbConnectionInfo : public ConnectionInfo, public XIMU3_UsbConnectionInfo
    {
    public:
        explicit UsbConnectionInfo(const std::string& portName) : XIMU3_UsbConnectionInfo()
        {
            stringCopy(port_name, portName.c_str(), sizeof(port_name));
        }

        explicit UsbConnectionInfo(const XIMU3_UsbConnectionInfo& connectionInfo) : XIMU3_UsbConnectionInfo()
        {
            stringCopy(port_name, connectionInfo.port_name, sizeof(port_name));
        }

        std::string toString() const override
        {
            return XIMU3_usb_connection_info_to_string(*this);
        }
    };

    class SerialConnectionInfo : public ConnectionInfo, public XIMU3_SerialConnectionInfo
    {
    public:
        SerialConnectionInfo(const std::string& portName, const uint32_t baudRate, const bool rtsCtsEnabled) : XIMU3_SerialConnectionInfo()
        {
            stringCopy(port_name, portName.c_str(), sizeof(port_name));
            baud_rate = baudRate;
            rts_cts_enabled = rtsCtsEnabled;
        }

        explicit SerialConnectionInfo(const XIMU3_SerialConnectionInfo& connectionInfo) : XIMU3_SerialConnectionInfo()
        {
            stringCopy(port_name, connectionInfo.port_name, sizeof(port_name));
            baud_rate = connectionInfo.baud_rate;
            rts_cts_enabled = connectionInfo.rts_cts_enabled;
        }

        std::string toString() const override
        {
            return XIMU3_serial_connection_info_to_string(*this);
        }
    };

    class TcpConnectionInfo : public ConnectionInfo, public XIMU3_TcpConnectionInfo
    {
    public:
        TcpConnectionInfo(const std::string& ipAddress, const uint16_t port_) : XIMU3_TcpConnectionInfo()
        {
            stringCopy(ip_address, ipAddress.c_str(), sizeof(ip_address));
            this->port = port_;
        }

        explicit TcpConnectionInfo(const XIMU3_TcpConnectionInfo& connectionInfo) : XIMU3_TcpConnectionInfo()
        {
            stringCopy(ip_address, connectionInfo.ip_address, sizeof(ip_address));
            port = connectionInfo.port;
        }

        std::string toString() const override
        {
            return XIMU3_tcp_connection_info_to_string(*this);
        }
    };

    class UdpConnectionInfo : public ConnectionInfo, public XIMU3_UdpConnectionInfo
    {
    public:
        UdpConnectionInfo(const std::string& ipAddress, const uint16_t sendPort, const uint16_t receivePort) : XIMU3_UdpConnectionInfo()
        {
            stringCopy(ip_address, ipAddress.c_str(), sizeof(ip_address));
            send_port = sendPort;
            receive_port = receivePort;
        }

        explicit UdpConnectionInfo(const XIMU3_UdpConnectionInfo& connectionInfo) : XIMU3_UdpConnectionInfo()
        {
            stringCopy(ip_address, connectionInfo.ip_address, sizeof(ip_address));
            send_port = connectionInfo.send_port;
            receive_port = connectionInfo.receive_port;
        }

        std::string toString() const override
        {
            return XIMU3_udp_connection_info_to_string(*this);
        }
    };

    class BluetoothConnectionInfo : public ConnectionInfo, public XIMU3_BluetoothConnectionInfo
    {
    public:
        explicit BluetoothConnectionInfo(const std::string& portName) : XIMU3_BluetoothConnectionInfo()
        {
            stringCopy(port_name, portName.c_str(), sizeof(port_name));
        }

        explicit BluetoothConnectionInfo(const XIMU3_BluetoothConnectionInfo& connectionInfo) : XIMU3_BluetoothConnectionInfo()
        {
            stringCopy(port_name, connectionInfo.port_name, sizeof(port_name));
        }

        std::string toString() const override
        {
            return XIMU3_bluetooth_connection_info_to_string(*this);
        }
    };

    class FileConnectionInfo : public ConnectionInfo, public XIMU3_FileConnectionInfo
    {
    public:
        explicit FileConnectionInfo(const std::string& filePath) : XIMU3_FileConnectionInfo()
        {
            stringCopy(file_path, filePath.c_str(), sizeof(file_path));
        }

        explicit FileConnectionInfo(const XIMU3_FileConnectionInfo& connectionInfo) : XIMU3_FileConnectionInfo()
        {
            stringCopy(file_path, connectionInfo.file_path, sizeof(file_path));
        }

        std::string toString() const override
        {
            return XIMU3_file_connection_info_to_string(*this);
        }
    };
} // namespace ximu3
