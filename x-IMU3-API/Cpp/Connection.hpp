#pragma once

#include "../C/Ximu3.h"
#include <cassert>
#include "CommandMessage.hpp"
#include "ConnectionConfig.hpp"
#include <cstring>
#include <functional>
#include "Helpers.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ximu3 {
    class Connection {
    public:
        explicit Connection(const ConnectionConfig &config) {
            if (auto *castConfig = dynamic_cast<const UsbConnectionConfig *>(&config)) {
                wrapped = XIMU3_connection_new_usb(*castConfig);
                return;
            }
            if (auto *castConfig = dynamic_cast<const SerialConnectionConfig *>(&config)) {
                wrapped = XIMU3_connection_new_serial(*castConfig);
                return;
            }
            if (auto *castConfig = dynamic_cast<const TcpConnectionConfig *>(&config)) {
                wrapped = XIMU3_connection_new_tcp(*castConfig);
                return;
            }
            if (auto *castConfig = dynamic_cast<const UdpConnectionConfig *>(&config)) {
                wrapped = XIMU3_connection_new_udp(*castConfig);
                return;
            }
            if (auto *castConfig = dynamic_cast<const BluetoothConnectionConfig *>(&config)) {
                wrapped = XIMU3_connection_new_bluetooth(*castConfig);
                return;
            }
            if (auto *castConfig = dynamic_cast<const FileConnectionConfig *>(&config)) {
                wrapped = XIMU3_connection_new_file(*castConfig);
                return;
            }
            if (auto *castConfig = dynamic_cast<const MuxConnectionConfig *>(&config)) {
                wrapped = XIMU3_connection_new_mux(castConfig->muxConnectionConfig);
                return;
            }
            assert(false);
        }

        ~Connection() {
            XIMU3_connection_free(wrapped);
        }

        XIMU3_Result open() {
            return XIMU3_connection_open(wrapped);
        }

        void openAsync(std::function<void(XIMU3_Result)> callback) {
            struct WrappedCallback {
                std::function<void(const XIMU3_Result)> callback;

                void operator()(XIMU3_Result result) const {
                    callback(result);
                    delete this;
                }
            };
            auto *const wrappedCallback = new WrappedCallback({callback});

            return XIMU3_connection_open_async(wrapped, Helpers::wrapCallable<XIMU3_Result>(*wrappedCallback), wrappedCallback);
        }

        void close() {
            XIMU3_connection_close(wrapped);
        }

        std::optional<XIMU3_PingResponse> ping() {
            return toOptional(XIMU3_connection_ping(wrapped));
        }

        void pingAsync(std::function<void(std::optional<XIMU3_PingResponse>)> callback) {
            struct WrappedCallback {
                std::function<void(std::optional<XIMU3_PingResponse>)> callback;

                void operator()(XIMU3_PingResponse response) const {
                    callback(toOptional(response));
                    delete this;
                }
            };
            auto *const wrappedCallback = new WrappedCallback({callback});

            return XIMU3_connection_ping_async(wrapped, Helpers::wrapCallable<XIMU3_PingResponse>(*wrappedCallback), wrappedCallback);
        }

        std::optional<CommandMessage> sendCommand(const std::string &command, const uint32_t retries = XIMU3_DEFAULT_RETRIES, const uint32_t timeout = XIMU3_DEFAULT_TIMEOUT) {
            return CommandMessage::from(XIMU3_connection_send_command(wrapped, command.data(), retries, timeout));
        }

        std::vector<std::optional<CommandMessage> > sendCommands(const std::vector<std::string> &commands, const uint32_t retries = XIMU3_DEFAULT_RETRIES, const uint32_t timeout = XIMU3_DEFAULT_TIMEOUT) {
            const auto charPtrVector = Helpers::toCharPtrVector(commands);
            return toVectorAndFree(XIMU3_connection_send_commands(wrapped, charPtrVector.data(), (uint32_t) charPtrVector.size(), retries, timeout));
        }

        void sendCommandAsync(const std::string &command, std::function<void(const std::optional<CommandMessage> &)> callback, const uint32_t retries = XIMU3_DEFAULT_RETRIES, const uint32_t timeout = XIMU3_DEFAULT_TIMEOUT) {
            struct WrappedCallback {
                std::function<void(const std::optional<CommandMessage> &)> callback;

                void operator()(XIMU3_CommandMessage response) const {
                    callback(CommandMessage::from(response));
                    delete this;
                }
            };
            auto *const wrappedCallback = new WrappedCallback({callback});

            XIMU3_connection_send_command_async(wrapped, command.data(), retries, timeout, Helpers::wrapCallable<XIMU3_CommandMessage>(*wrappedCallback), wrappedCallback);
        }

        void sendCommandsAsync(const std::vector<std::string> &commands, std::function<void(const std::vector<std::optional<CommandMessage> > &)> callback, const uint32_t retries = XIMU3_DEFAULT_RETRIES, const uint32_t timeout = XIMU3_DEFAULT_TIMEOUT) {
            struct WrappedCallback {
                std::function<void(const std::vector<std::optional<CommandMessage> > &)> callback;

                void operator()(XIMU3_CommandMessages responses) const {
                    callback(toVectorAndFree(responses));
                    delete this;
                }
            };
            auto *const wrappedCallback = new WrappedCallback({callback});

            const auto charPtrVector = Helpers::toCharPtrVector(commands);
            XIMU3_connection_send_commands_async(wrapped, charPtrVector.data(), (uint32_t) charPtrVector.size(), retries, timeout, Helpers::wrapCallable<XIMU3_CommandMessages>(*wrappedCallback), wrappedCallback);
        }

        std::unique_ptr<ConnectionConfig> getConfig() {
            switch (XIMU3_connection_get_type(wrapped)) {
                case XIMU3_ConnectionTypeUsb:
                    return std::make_unique<UsbConnectionConfig>(XIMU3_connection_get_config_usb(wrapped));
                case XIMU3_ConnectionTypeSerial:
                    return std::make_unique<SerialConnectionConfig>(XIMU3_connection_get_config_serial(wrapped));
                case XIMU3_ConnectionTypeTcp:
                    return std::make_unique<TcpConnectionConfig>(XIMU3_connection_get_config_tcp(wrapped));
                case XIMU3_ConnectionTypeUdp:
                    return std::make_unique<UdpConnectionConfig>(XIMU3_connection_get_config_udp(wrapped));
                case XIMU3_ConnectionTypeBluetooth:
                    return std::make_unique<BluetoothConnectionConfig>(XIMU3_connection_get_config_bluetooth(wrapped));
                case XIMU3_ConnectionTypeFile:
                    return std::make_unique<FileConnectionConfig>(XIMU3_connection_get_config_file(wrapped));
                case XIMU3_ConnectionTypeMux:
                    return std::make_unique<MuxConnectionConfig>(XIMU3_connection_get_config_mux(wrapped));
            }
            return nullptr;
        }

        XIMU3_Statistics getStatistics() {
            return XIMU3_connection_get_statistics(wrapped);
        }

        uint64_t addReceiveErrorCallback(std::function<void(XIMU3_ReceiveError)> &callback) {
            return XIMU3_connection_add_receive_error_callback(wrapped, Helpers::wrapCallable<XIMU3_ReceiveError>(callback), &callback);
        }

        uint64_t addStatisticsCallback(std::function<void(XIMU3_Statistics)> &callback) {
            return XIMU3_connection_add_statistics_callback(wrapped, Helpers::wrapCallable<XIMU3_Statistics>(callback), &callback);
        }

        // Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py
        uint64_t addInertialCallback(std::function<void(XIMU3_InertialMessage)> &callback) {
            return XIMU3_connection_add_inertial_callback(wrapped, Helpers::wrapCallable<XIMU3_InertialMessage>(callback), &callback);
        }

        uint64_t addMagnetometerCallback(std::function<void(XIMU3_MagnetometerMessage)> &callback) {
            return XIMU3_connection_add_magnetometer_callback(wrapped, Helpers::wrapCallable<XIMU3_MagnetometerMessage>(callback), &callback);
        }

        uint64_t addQuaternionCallback(std::function<void(XIMU3_QuaternionMessage)> &callback) {
            return XIMU3_connection_add_quaternion_callback(wrapped, Helpers::wrapCallable<XIMU3_QuaternionMessage>(callback), &callback);
        }

        uint64_t addRotationMatrixCallback(std::function<void(XIMU3_RotationMatrixMessage)> &callback) {
            return XIMU3_connection_add_rotation_matrix_callback(wrapped, Helpers::wrapCallable<XIMU3_RotationMatrixMessage>(callback), &callback);
        }

        uint64_t addEulerAnglesCallback(std::function<void(XIMU3_EulerAnglesMessage)> &callback) {
            return XIMU3_connection_add_euler_angles_callback(wrapped, Helpers::wrapCallable<XIMU3_EulerAnglesMessage>(callback), &callback);
        }

        uint64_t addLinearAccelerationCallback(std::function<void(XIMU3_LinearAccelerationMessage)> &callback) {
            return XIMU3_connection_add_linear_acceleration_callback(wrapped, Helpers::wrapCallable<XIMU3_LinearAccelerationMessage>(callback), &callback);
        }

        uint64_t addEarthAccelerationCallback(std::function<void(XIMU3_EarthAccelerationMessage)> &callback) {
            return XIMU3_connection_add_earth_acceleration_callback(wrapped, Helpers::wrapCallable<XIMU3_EarthAccelerationMessage>(callback), &callback);
        }

        uint64_t addAhrsStatusCallback(std::function<void(XIMU3_AhrsStatusMessage)> &callback) {
            return XIMU3_connection_add_ahrs_status_callback(wrapped, Helpers::wrapCallable<XIMU3_AhrsStatusMessage>(callback), &callback);
        }

        uint64_t addHighGAccelerometerCallback(std::function<void(XIMU3_HighGAccelerometerMessage)> &callback) {
            return XIMU3_connection_add_high_g_accelerometer_callback(wrapped, Helpers::wrapCallable<XIMU3_HighGAccelerometerMessage>(callback), &callback);
        }

        uint64_t addTemperatureCallback(std::function<void(XIMU3_TemperatureMessage)> &callback) {
            return XIMU3_connection_add_temperature_callback(wrapped, Helpers::wrapCallable<XIMU3_TemperatureMessage>(callback), &callback);
        }

        uint64_t addBatteryCallback(std::function<void(XIMU3_BatteryMessage)> &callback) {
            return XIMU3_connection_add_battery_callback(wrapped, Helpers::wrapCallable<XIMU3_BatteryMessage>(callback), &callback);
        }

        uint64_t addRssiCallback(std::function<void(XIMU3_RssiMessage)> &callback) {
            return XIMU3_connection_add_rssi_callback(wrapped, Helpers::wrapCallable<XIMU3_RssiMessage>(callback), &callback);
        }

        uint64_t addSerialAccessoryCallback(std::function<void(XIMU3_SerialAccessoryMessage)> &callback) {
            return XIMU3_connection_add_serial_accessory_callback(wrapped, Helpers::wrapCallable<XIMU3_SerialAccessoryMessage>(callback), &callback);
        }

        uint64_t addNotificationCallback(std::function<void(XIMU3_NotificationMessage)> &callback) {
            return XIMU3_connection_add_notification_callback(wrapped, Helpers::wrapCallable<XIMU3_NotificationMessage>(callback), &callback);
        }

        uint64_t addErrorCallback(std::function<void(XIMU3_ErrorMessage)> &callback) {
            return XIMU3_connection_add_error_callback(wrapped, Helpers::wrapCallable<XIMU3_ErrorMessage>(callback), &callback);
        }

        // End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

        uint64_t addEndOfFileCallback(std::function<void()> &callback) {
            return XIMU3_connection_add_end_of_file_callback(wrapped, Helpers::wrapCallable<>(callback), &callback);
        }

        void removeCallback(const uint64_t callbackId) {
            XIMU3_connection_remove_callback(wrapped, callbackId);
        }

    private:
        friend class DataLogger;
        friend class KeepOpen;
        friend class MuxConnectionConfig;

        XIMU3_Connection *wrapped;

        static std::optional<XIMU3_PingResponse> toOptional(const XIMU3_PingResponse &response) {
            if (std::strlen(response.interface) == 0) {
                return {};
            }

            return response;
        }

        static std::vector<std::optional<CommandMessage> > toVectorAndFree(const XIMU3_CommandMessages &responses) {
            std::vector<std::optional<CommandMessage> > vector;

            for (const auto &response: Helpers::toVector<XIMU3_CommandMessage>(responses)) {
                vector.push_back(CommandMessage::from(response));
            }

            XIMU3_command_messages_free(responses);

            return vector;
        }
    };
} // namespace ximu3
