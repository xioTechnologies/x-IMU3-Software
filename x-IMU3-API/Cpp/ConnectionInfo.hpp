#pragma once

#include "../C/Ximu3.h"
#include <cstring>
#include <memory>
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

        static std::shared_ptr<ConnectionInfo> from(const XIMU3_Device& device);

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

        explicit UsbConnectionInfo(const XIMU3_UsbConnectionInfo& connectionInfo) : XIMU3_UsbConnectionInfo(connectionInfo)
        {
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

        explicit SerialConnectionInfo(const XIMU3_SerialConnectionInfo& connectionInfo) : XIMU3_SerialConnectionInfo(connectionInfo)
        {
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

        explicit TcpConnectionInfo(const XIMU3_TcpConnectionInfo& connectionInfo) : XIMU3_TcpConnectionInfo(connectionInfo)
        {
        }

        explicit TcpConnectionInfo(const XIMU3_NetworkAnnouncementMessage& message) : XIMU3_TcpConnectionInfo(XIMU3_network_announcement_message_to_tcp_connection_info(message))
        {
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

        explicit UdpConnectionInfo(const XIMU3_UdpConnectionInfo& connectionInfo) : XIMU3_UdpConnectionInfo(connectionInfo)
        {
        }

        explicit UdpConnectionInfo(const XIMU3_NetworkAnnouncementMessage& message) : XIMU3_UdpConnectionInfo(XIMU3_network_announcement_message_to_udp_connection_info(message))
        {
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

        explicit BluetoothConnectionInfo(const XIMU3_BluetoothConnectionInfo& connectionInfo) : XIMU3_BluetoothConnectionInfo(connectionInfo)
        {
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

        explicit FileConnectionInfo(const XIMU3_FileConnectionInfo& connectionInfo) : XIMU3_FileConnectionInfo(connectionInfo)
        {
        }

        std::string toString() const override
        {
            return XIMU3_file_connection_info_to_string(*this);
        }
    };

    inline std::shared_ptr<ConnectionInfo> ConnectionInfo::from(const XIMU3_Device& device)
    {
        switch (device.connection_type)
        {
            case XIMU3_ConnectionTypeUsb:
                return std::make_shared<UsbConnectionInfo>(device.usb_connection_info);
            case XIMU3_ConnectionTypeSerial:
                return std::make_shared<SerialConnectionInfo>(device.serial_connection_info);
            case XIMU3_ConnectionTypeBluetooth:
                return std::make_shared<BluetoothConnectionInfo>(device.bluetooth_connection_info);
            case XIMU3_ConnectionTypeTcp:
            case XIMU3_ConnectionTypeUdp:
            case XIMU3_ConnectionTypeFile:
                break;
        }
        return {};
    }
} // namespace ximu3
