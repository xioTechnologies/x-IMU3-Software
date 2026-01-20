#pragma once

#include "../C/Ximu3.h"
#include <cstring>
#include <memory>
#include <string>

namespace ximu3 {
    class ConnectionConfig {
    public:
        ConnectionConfig() = default;

        ConnectionConfig(const ConnectionConfig &) = delete;

        ConnectionConfig &operator=(const ConnectionConfig &) = delete;

        virtual ~ConnectionConfig() = default;

        virtual std::string toString() const = 0;

        static std::shared_ptr<ConnectionConfig> from(const XIMU3_Device &device);

    protected:
        static void stringCopy(char *destination, const char *source, const size_t destinationSize) {
            strncpy(destination, source, destinationSize);
            destination[destinationSize - 1] = '\0';
        }
    };

    class UsbConnectionConfig : public ConnectionConfig, public XIMU3_UsbConnectionConfig {
    public:
        explicit UsbConnectionConfig(const std::string &portName) : XIMU3_UsbConnectionConfig() {
            stringCopy(port_name, portName.c_str(), sizeof(port_name));
        }

        explicit UsbConnectionConfig(const XIMU3_UsbConnectionConfig &config) : XIMU3_UsbConnectionConfig(config) {
        }

        std::string toString() const override {
            return XIMU3_usb_connection_config_to_string(*this);
        }
    };

    class SerialConnectionConfig : public ConnectionConfig, public XIMU3_SerialConnectionConfig {
    public:
        SerialConnectionConfig(const std::string &portName, const uint32_t baudRate, const bool rtsCtsEnabled) : XIMU3_SerialConnectionConfig() {
            stringCopy(port_name, portName.c_str(), sizeof(port_name));
            baud_rate = baudRate;
            rts_cts_enabled = rtsCtsEnabled;
        }

        explicit SerialConnectionConfig(const XIMU3_SerialConnectionConfig &config) : XIMU3_SerialConnectionConfig(config) {
        }

        std::string toString() const override {
            return XIMU3_serial_connection_config_to_string(*this);
        }
    };

    class TcpConnectionConfig : public ConnectionConfig, public XIMU3_TcpConnectionConfig {
    public:
        TcpConnectionConfig(const std::string &ipAddress, const uint16_t port_) : XIMU3_TcpConnectionConfig() {
            stringCopy(ip_address, ipAddress.c_str(), sizeof(ip_address));
            this->port = port_;
        }

        explicit TcpConnectionConfig(const XIMU3_TcpConnectionConfig &config) : XIMU3_TcpConnectionConfig(config) {
        }

        explicit TcpConnectionConfig(const XIMU3_NetworkAnnouncementMessage &message) : XIMU3_TcpConnectionConfig(XIMU3_network_announcement_message_to_tcp_connection_config(message)) {
        }

        std::string toString() const override {
            return XIMU3_tcp_connection_config_to_string(*this);
        }
    };

    class UdpConnectionConfig : public ConnectionConfig, public XIMU3_UdpConnectionConfig {
    public:
        UdpConnectionConfig(const std::string &ipAddress, const uint16_t sendPort, const uint16_t receivePort) : XIMU3_UdpConnectionConfig() {
            stringCopy(ip_address, ipAddress.c_str(), sizeof(ip_address));
            send_port = sendPort;
            receive_port = receivePort;
        }

        explicit UdpConnectionConfig(const XIMU3_UdpConnectionConfig &config) : XIMU3_UdpConnectionConfig(config) {
        }

        explicit UdpConnectionConfig(const XIMU3_NetworkAnnouncementMessage &message) : XIMU3_UdpConnectionConfig(XIMU3_network_announcement_message_to_udp_connection_config(message)) {
        }

        std::string toString() const override {
            return XIMU3_udp_connection_config_to_string(*this);
        }
    };

    class BluetoothConnectionConfig : public ConnectionConfig, public XIMU3_BluetoothConnectionConfig {
    public:
        explicit BluetoothConnectionConfig(const std::string &portName) : XIMU3_BluetoothConnectionConfig() {
            stringCopy(port_name, portName.c_str(), sizeof(port_name));
        }

        explicit BluetoothConnectionConfig(const XIMU3_BluetoothConnectionConfig &config) : XIMU3_BluetoothConnectionConfig(config) {
        }

        std::string toString() const override {
            return XIMU3_bluetooth_connection_config_to_string(*this);
        }
    };

    class FileConnectionConfig : public ConnectionConfig, public XIMU3_FileConnectionConfig {
    public:
        explicit FileConnectionConfig(const std::string &filePath) : XIMU3_FileConnectionConfig() {
            stringCopy(file_path, filePath.c_str(), sizeof(file_path));
        }

        explicit FileConnectionConfig(const XIMU3_FileConnectionConfig &config) : XIMU3_FileConnectionConfig(config) {
        }

        std::string toString() const override {
            return XIMU3_file_connection_config_to_string(*this);
        }
    };

    class Connection;

    class MuxConnectionConfig : public ConnectionConfig {
    public:
        MuxConnectionConfig(const std::uint8_t channel, const Connection &connection);

        MuxConnectionConfig(XIMU3_MuxConnectionConfig *config) {
            muxConnectionConfig = config;
        }

        ~MuxConnectionConfig() override {
            XIMU3_mux_connection_config_free(muxConnectionConfig);
        }

        std::string toString() const override {
            return XIMU3_mux_connection_config_to_string(muxConnectionConfig);
        }

    private:
        XIMU3_MuxConnectionConfig *muxConnectionConfig;

        friend class Connection;
    };

    inline std::shared_ptr<ConnectionConfig> ConnectionConfig::from(const XIMU3_Device &device) {
        switch (device.connection_type) {
            case XIMU3_ConnectionTypeUsb:
                return std::make_shared<UsbConnectionConfig>(device.usb_connection_config);
            case XIMU3_ConnectionTypeSerial:
                return std::make_shared<SerialConnectionConfig>(device.serial_connection_config);
            case XIMU3_ConnectionTypeBluetooth:
                return std::make_shared<BluetoothConnectionConfig>(device.bluetooth_connection_config);
            case XIMU3_ConnectionTypeTcp:
            case XIMU3_ConnectionTypeUdp:
            case XIMU3_ConnectionTypeFile:
            case XIMU3_ConnectionTypeMux:
                break;
        }
        return {};
    }
} // namespace ximu3
