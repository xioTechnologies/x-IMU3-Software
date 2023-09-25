#pragma once

#include "../C/Ximu3.h"
#include <cassert>
#include "ConnectionInfo.hpp"
#include <functional>
#include "Helpers.hpp"
#include <memory>
#include <string>
#include <vector>

namespace ximu3
{
    class Connection
    {
    public:
        explicit Connection(ConnectionInfo* connectionInfo) : Connection(*connectionInfo)
        {
        }

        explicit Connection(const ConnectionInfo& connectionInfo)
        {
            if (auto* castConnectionInfo = dynamic_cast<const XIMU3_UsbConnectionInfo*> (&connectionInfo))
            {
                connection = XIMU3_connection_new_usb(*castConnectionInfo);
                return;
            }
            if (auto* castConnectionInfo = dynamic_cast<const XIMU3_SerialConnectionInfo*> (&connectionInfo))
            {
                connection = XIMU3_connection_new_serial(*castConnectionInfo);
                return;
            }
            if (auto* castConnectionInfo = dynamic_cast<const XIMU3_TcpConnectionInfo*> (&connectionInfo))
            {
                connection = XIMU3_connection_new_tcp(*castConnectionInfo);
                return;
            }
            if (auto* castConnectionInfo = dynamic_cast<const XIMU3_UdpConnectionInfo*> (&connectionInfo))
            {
                connection = XIMU3_connection_new_udp(*castConnectionInfo);
                return;
            }
            if (auto* castConnectionInfo = dynamic_cast<const XIMU3_BluetoothConnectionInfo*> (&connectionInfo))
            {
                connection = XIMU3_connection_new_bluetooth(*castConnectionInfo);
                return;
            }
            if (auto* castConnectionInfo = dynamic_cast<const XIMU3_FileConnectionInfo*> (&connectionInfo))
            {
                connection = XIMU3_connection_new_file(*castConnectionInfo);
                return;
            }
            assert(false);
        }

        ~Connection()
        {
            XIMU3_connection_free(connection);
        }

        XIMU3_Result open()
        {
            return XIMU3_connection_open(connection);
        }

        void openAsync(std::function<void(XIMU3_Result)> callback)
        {
            struct WrappedCallback
            {
                std::function<void(const XIMU3_Result)> callback;

                void operator()(XIMU3_Result result) const
                {
                    callback(result);
                    delete this;
                }
            };
            auto* const wrappedCallback = new WrappedCallback({ callback });

            return XIMU3_connection_open_async(connection, Helpers::wrapCallable<XIMU3_Result>(*wrappedCallback), wrappedCallback);
        }

        void close()
        {
            XIMU3_connection_close(connection);
        }

        XIMU3_PingResponse ping()
        {
            return XIMU3_connection_ping(connection);
        }

        std::vector<std::string> sendCommands(const std::vector<std::string>& commands, const uint32_t retries, const uint32_t timeout)
        {
            const auto charPtrVector = toCharPtrVector(commands);
            return Helpers::toVectorAndFree(XIMU3_connection_send_commands(connection, charPtrVector.data(), (uint32_t) charPtrVector.size(), retries, timeout));
        }

        void sendCommandsAsync(const std::vector<std::string>& commands, const uint32_t retries, const uint32_t timeout, std::function<void(const std::vector<std::string>&)> callback)
        {
            struct WrappedCallback
            {
                std::function<void(const std::vector<std::string>&)> callback;

                void operator()(XIMU3_CharArrays charArrays) const
                {
                    callback(Helpers::toVectorAndFree(charArrays));
                    delete this;
                }
            };
            auto* const wrappedCallback = new WrappedCallback({ callback });

            const auto charPtrVector = toCharPtrVector(commands);
            XIMU3_connection_send_commands_async(connection, charPtrVector.data(), (uint32_t) charPtrVector.size(), retries, timeout, Helpers::wrapCallable<XIMU3_CharArrays>(*wrappedCallback), wrappedCallback);
        }

        std::unique_ptr<ConnectionInfo> getInfo()
        {
            switch (XIMU3_connection_get_type(connection))
            {
                case XIMU3_ConnectionTypeUsb:
                    return std::make_unique<UsbConnectionInfo>(XIMU3_connection_get_info_usb(connection));
                case XIMU3_ConnectionTypeSerial:
                    return std::make_unique<SerialConnectionInfo>(XIMU3_connection_get_info_serial(connection));
                case XIMU3_ConnectionTypeTcp:
                    return std::make_unique<TcpConnectionInfo>(XIMU3_connection_get_info_tcp(connection));
                case XIMU3_ConnectionTypeUdp:
                    return std::make_unique<UdpConnectionInfo>(XIMU3_connection_get_info_udp(connection));
                case XIMU3_ConnectionTypeBluetooth:
                    return std::make_unique<BluetoothConnectionInfo>(XIMU3_connection_get_info_bluetooth(connection));
                case XIMU3_ConnectionTypeFile:
                    return std::make_unique<FileConnectionInfo>(XIMU3_connection_get_info_file(connection));
            }
            return nullptr;
        }

        XIMU3_Statistics getStatistics()
        {
            return XIMU3_connection_get_statistics(connection);
        }

        uint64_t addDecodeErrorCallback(std::function<void(XIMU3_DecodeError)>& callback)
        {
            return XIMU3_connection_add_decode_error_callback(connection, Helpers::wrapCallable<XIMU3_DecodeError>(callback), &callback);
        }

        uint64_t addStatisticsCallback(std::function<void(XIMU3_Statistics)>& callback)
        {
            return XIMU3_connection_add_statistics_callback(connection, Helpers::wrapCallable<XIMU3_Statistics>(callback), &callback);
        }

        // Start of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

        uint64_t addInertialCallback(std::function<void(XIMU3_InertialMessage)>& callback)
        {
            return XIMU3_connection_add_inertial_callback(connection, Helpers::wrapCallable<XIMU3_InertialMessage>(callback), &callback);
        }

        uint64_t addMagnetometerCallback(std::function<void(XIMU3_MagnetometerMessage)>& callback)
        {
            return XIMU3_connection_add_magnetometer_callback(connection, Helpers::wrapCallable<XIMU3_MagnetometerMessage>(callback), &callback);
        }

        uint64_t addQuaternionCallback(std::function<void(XIMU3_QuaternionMessage)>& callback)
        {
            return XIMU3_connection_add_quaternion_callback(connection, Helpers::wrapCallable<XIMU3_QuaternionMessage>(callback), &callback);
        }

        uint64_t addRotationMatrixCallback(std::function<void(XIMU3_RotationMatrixMessage)>& callback)
        {
            return XIMU3_connection_add_rotation_matrix_callback(connection, Helpers::wrapCallable<XIMU3_RotationMatrixMessage>(callback), &callback);
        }

        uint64_t addEulerAnglesCallback(std::function<void(XIMU3_EulerAnglesMessage)>& callback)
        {
            return XIMU3_connection_add_euler_angles_callback(connection, Helpers::wrapCallable<XIMU3_EulerAnglesMessage>(callback), &callback);
        }

        uint64_t addLinearAccelerationCallback(std::function<void(XIMU3_LinearAccelerationMessage)>& callback)
        {
            return XIMU3_connection_add_linear_acceleration_callback(connection, Helpers::wrapCallable<XIMU3_LinearAccelerationMessage>(callback), &callback);
        }

        uint64_t addEarthAccelerationCallback(std::function<void(XIMU3_EarthAccelerationMessage)>& callback)
        {
            return XIMU3_connection_add_earth_acceleration_callback(connection, Helpers::wrapCallable<XIMU3_EarthAccelerationMessage>(callback), &callback);
        }

        uint64_t addAhrsStatusCallback(std::function<void(XIMU3_AhrsStatusMessage)>& callback)
        {
            return XIMU3_connection_add_ahrs_status_callback(connection, Helpers::wrapCallable<XIMU3_AhrsStatusMessage>(callback), &callback);
        }

        uint64_t addHighGAccelerometerCallback(std::function<void(XIMU3_HighGAccelerometerMessage)>& callback)
        {
            return XIMU3_connection_add_high_g_accelerometer_callback(connection, Helpers::wrapCallable<XIMU3_HighGAccelerometerMessage>(callback), &callback);
        }

        uint64_t addTemperatureCallback(std::function<void(XIMU3_TemperatureMessage)>& callback)
        {
            return XIMU3_connection_add_temperature_callback(connection, Helpers::wrapCallable<XIMU3_TemperatureMessage>(callback), &callback);
        }

        uint64_t addBatteryCallback(std::function<void(XIMU3_BatteryMessage)>& callback)
        {
            return XIMU3_connection_add_battery_callback(connection, Helpers::wrapCallable<XIMU3_BatteryMessage>(callback), &callback);
        }

        uint64_t addRssiCallback(std::function<void(XIMU3_RssiMessage)>& callback)
        {
            return XIMU3_connection_add_rssi_callback(connection, Helpers::wrapCallable<XIMU3_RssiMessage>(callback), &callback);
        }

        uint64_t addSerialAccessoryCallback(std::function<void(XIMU3_SerialAccessoryMessage)>& callback)
        {
            return XIMU3_connection_add_serial_accessory_callback(connection, Helpers::wrapCallable<XIMU3_SerialAccessoryMessage>(callback), &callback);
        }

        uint64_t addNotificationCallback(std::function<void(XIMU3_NotificationMessage)>& callback)
        {
            return XIMU3_connection_add_notification_callback(connection, Helpers::wrapCallable<XIMU3_NotificationMessage>(callback), &callback);
        }

        uint64_t addErrorCallback(std::function<void(XIMU3_ErrorMessage)>& callback)
        {
            return XIMU3_connection_add_error_callback(connection, Helpers::wrapCallable<XIMU3_ErrorMessage>(callback), &callback);
        }
        // End of code block #0 generated by x-IMU3-API/Rust/src/data_messages/generate_data_messages.py

        void removeCallback(const uint64_t callbackID)
        {
            XIMU3_connection_remove_callback(connection, callbackID);
        }

    private:
        friend class DataLogger;

        XIMU3_Connection* connection;

        static std::vector<const char*> toCharPtrVector(const std::vector<std::string>& stringVector)
        {
            std::vector<const char*> charPtrVector(stringVector.size());
            for (size_t index = 0; index < charPtrVector.size(); index++)
            {
                charPtrVector[index] = stringVector[index].c_str();
            }
            return charPtrVector;
        }
    };
} // namespace ximu3
