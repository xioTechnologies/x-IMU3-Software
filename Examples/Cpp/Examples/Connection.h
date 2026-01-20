#pragma once

#include "../Helpers.hpp"
#include <chrono>
#include <inttypes.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include "Ximu3.hpp"

#define TIMESTAMP_FORMAT "%8" PRIu64 " us"
#define UINT32_FORMAT " %8" PRIu32
#define UINT64_FORMAT " %8" PRIu64
#define FLOAT_FORMAT " %8.3f"
#define STRING_FORMAT " \"%s\""

class Connection {
protected:
    void run(const ximu3::ConnectionConfig &config) {
        // Create connection
        ximu3::Connection connection(config);

        connection.addReceiveErrorCallback(receiveErrorCallback);
        connection.addStatisticsCallback(statisticsCallback);

        if (helpers::yesOrNo("Print data messages?")) {
            connection.addInertialCallback(inertialCallback);
            connection.addMagnetometerCallback(magnetometerCallback);
            connection.addQuaternionCallback(quaternionCallback);
            connection.addRotationMatrixCallback(rotationMatrixCallback);
            connection.addEulerAnglesCallback(eulerAnglesCallback);
            connection.addLinearAccelerationCallback(linearAccelerationCallback);
            connection.addEarthAccelerationCallback(earthAccelerationCallback);
            connection.addAhrsStatusCallback(ahrsStatusCallback);
            connection.addHighGAccelerometerCallback(highGAccelerometerCallback);
            connection.addTemperatureCallback(temperatureCallback);
            connection.addBatteryCallback(batteryCallback);
            connection.addRssiCallback(rssiCallback);
            connection.addSerialAccessoryCallback(serialAccessoryCallback);
            connection.addNotificationCallback(notificationCallback);
            connection.addErrorCallback(errorCallback);
        }
        connection.addEndOfFileCallback(endOfFileCallback);

        // Open connection
        const auto result = connection.open();

        if (result != ximu3::XIMU3_ResultOk) {
            std::cout << "Unable to open " << connection.getConfig()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        // Send command to strobe LED
        connection.sendCommand("{\"strobe\":null}");

        // Close connection
        std::this_thread::sleep_for(std::chrono::seconds(60));

        connection.close();
    }

private:
    std::function<void(ximu3::XIMU3_ReceiveError error)> receiveErrorCallback = [](auto error) {
        std::cout << ximu3::XIMU3_receive_error_to_string(error) << std::endl;
    };

    std::function<void(ximu3::XIMU3_Statistics statistics)> statisticsCallback = [](auto statistics) {
        printf(TIMESTAMP_FORMAT UINT64_FORMAT " bytes" UINT32_FORMAT " bytes/s" UINT64_FORMAT " messages" UINT32_FORMAT " messages/s" UINT64_FORMAT " errors" UINT32_FORMAT " errors/s\n",
               statistics.timestamp,
               statistics.data_total,
               statistics.data_rate,
               statistics.message_total,
               statistics.message_rate,
               statistics.error_total,
               statistics.error_rate);
        // std::cout << ximu3::XIMU3_statistics_to_string(statistics) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_InertialMessage message)> inertialCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT " deg/s" FLOAT_FORMAT " deg/s" FLOAT_FORMAT " deg/s" FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
               message.timestamp,
               message.gyroscope_x,
               message.gyroscope_y,
               message.gyroscope_z,
               message.accelerometer_x,
               message.accelerometer_y,
               message.accelerometer_z);
        // std::cout << ximu3::XIMU3_inertial_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_MagnetometerMessage message)> magnetometerCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT " a.u." FLOAT_FORMAT " a.u." FLOAT_FORMAT " a.u.\n",
               message.timestamp,
               message.x,
               message.y,
               message.z);
        // std::cout << ximu3::XIMU3_magnetometer_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_QuaternionMessage message)> quaternionCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT "\n",
               message.timestamp,
               message.w,
               message.x,
               message.y,
               message.z);
        // std::cout << ximu3::XIMU3_quaternion_message_to_string(message) << std::endl; // alternative to above
        std::cout << ximu3::XIMU3_euler_angles_message_to_string(ximu3::XIMU3_quaternion_message_to_euler_angles_message(message)) << std::endl;
    };

    std::function<void(ximu3::XIMU3_RotationMatrixMessage message)> rotationMatrixCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT "\n",
               message.timestamp,
               message.xx,
               message.xy,
               message.xz,
               message.yx,
               message.yy,
               message.yz,
               message.zx,
               message.zy,
               message.zz);
        // std::cout << ximu3::XIMU3_rotation_matrix_message_to_string(message) << std::endl; // alternative to above
        std::cout << ximu3::XIMU3_euler_angles_message_to_string(ximu3::XIMU3_rotation_matrix_message_to_euler_angles_message(message)) << std::endl;
    };

    std::function<void(ximu3::XIMU3_EulerAnglesMessage message)> eulerAnglesCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT " deg" FLOAT_FORMAT " deg" FLOAT_FORMAT " deg\n",
               message.timestamp,
               message.roll,
               message.pitch,
               message.yaw);
        // std::cout << ximu3::XIMU3_euler_angles_message_to_string(message) << std::endl; // alternative to above
        std::cout << ximu3::XIMU3_quaternion_message_to_string(ximu3::XIMU3_euler_angles_message_to_quaternion_message(message)) << std::endl;
    };

    std::function<void(ximu3::XIMU3_LinearAccelerationMessage message)> linearAccelerationCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
               message.timestamp,
               message.quaternion_w,
               message.quaternion_x,
               message.quaternion_y,
               message.quaternion_z,
               message.acceleration_x,
               message.acceleration_y,
               message.acceleration_z);
        // std::cout << ximu3::XIMU3_linear_acceleration_message_to_string(message) << std::endl; // alternative to above
        std::cout << ximu3::XIMU3_euler_angles_message_to_string(ximu3::XIMU3_linear_acceleration_message_to_euler_angles_message(message)) << std::endl;
    };

    std::function<void(ximu3::XIMU3_EarthAccelerationMessage message)> earthAccelerationCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
               message.timestamp,
               message.quaternion_w,
               message.quaternion_x,
               message.quaternion_y,
               message.quaternion_z,
               message.acceleration_x,
               message.acceleration_y,
               message.acceleration_z);
        // std::cout << ximu3::XIMU3_earth_acceleration_message_to_string(message) << std::endl; // alternative to above
        std::cout << ximu3::XIMU3_euler_angles_message_to_string(ximu3::XIMU3_earth_acceleration_message_to_euler_angles_message(message)) << std::endl;
    };

    std::function<void(ximu3::XIMU3_AhrsStatusMessage message)> ahrsStatusCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT "\n",
               message.timestamp,
               message.initialising,
               message.angular_rate_recovery,
               message.acceleration_recovery,
               message.magnetic_recovery);
        // std::cout << ximu3::XIMU3_ahrs_status_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_HighGAccelerometerMessage message)> highGAccelerometerCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
               message.timestamp,
               message.x,
               message.y,
               message.z);
        // std::cout << ximu3::XIMU3_high_g_accelerometer_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_TemperatureMessage message)> temperatureCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT " degC\n",
               message.timestamp,
               message.temperature);
        // std::cout << ximu3::XIMU3_temperature_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_BatteryMessage message)> batteryCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT " %%" FLOAT_FORMAT " V" FLOAT_FORMAT " (%s)\n",
               message.timestamp,
               message.percentage,
               message.voltage,
               message.charging_status,
               ximu3::XIMU3_charging_status_to_string(ximu3::XIMU3_charging_status_from_float(message.charging_status)));
        // std::cout << ximu3::XIMU3_battery_message_to_string(message) << " (" << ximu3::XIMU3_charging_status_to_string(ximu3::XIMU3_charging_status_from_float(message.charging_status)) << ")" << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_RssiMessage message)> rssiCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT FLOAT_FORMAT " %%" FLOAT_FORMAT " dBm\n",
               message.timestamp,
               message.percentage,
               message.power);
        // std::cout << ximu3::XIMU3_rssi_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_SerialAccessoryMessage message)> serialAccessoryCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
               message.timestamp,
               message.char_array);
        // std::cout << ximu3::XIMU3_serial_accessory_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_NotificationMessage message)> notificationCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
               message.timestamp,
               message.char_array);
        // std::cout << ximu3::XIMU3_notification_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void(ximu3::XIMU3_ErrorMessage message)> errorCallback = [](auto message) {
        printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
               message.timestamp,
               message.char_array);
        // std::cout << ximu3::XIMU3_error_message_to_string(message) << std::endl; // alternative to above
    };

    std::function<void()> endOfFileCallback = [] {
        std::cout << "End of file" << std::endl;
    };
};
