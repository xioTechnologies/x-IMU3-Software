#include "../helpers.h"
#include "connection.h"
#include <inttypes.h>
#include <stdio.h>

#define TIMESTAMP_FORMAT "%8" PRIu64 " us"
#define UINT32_FORMAT " %8" PRIu32
#define UINT64_FORMAT " %8" PRIu64
#define FLOAT_FORMAT " %8.3f"
#define STRING_FORMAT " \"%s\""

void receive_error_callback(const XIMU3_ReceiveError error, void *context);

void statistics_callback(const XIMU3_Statistics statistics, void *context);

void inertial_callback(const XIMU3_InertialMessage message, void *context);

void magnetometer_callback(const XIMU3_MagnetometerMessage message, void *context);

void quaternion_callback(const XIMU3_QuaternionMessage message, void *context);

void rotation_matrix_callback(const XIMU3_RotationMatrixMessage message, void *context);

void euler_angles_callback(const XIMU3_EulerAnglesMessage message, void *context);

void linear_acceleration_callback(const XIMU3_LinearAccelerationMessage message, void *context);

void earth_acceleration_callback(const XIMU3_EarthAccelerationMessage message, void *context);

void ahrs_status_callback(const XIMU3_AhrsStatusMessage message, void *context);

void high_g_accelerometer_callback(const XIMU3_HighGAccelerometerMessage message, void *context);

void temperature_callback(const XIMU3_TemperatureMessage message, void *context);

void battery_callback(const XIMU3_BatteryMessage message, void *context);

void rssi_callback(const XIMU3_RssiMessage message, void *context);

void serial_accessory_callback(const XIMU3_SerialAccessoryMessage message, void *context);

void notification_callback(const XIMU3_NotificationMessage message, void *context);

void error_callback(const XIMU3_ErrorMessage message, void *context);

void end_of_file_callback(void *context);

void run(XIMU3_Connection *const connection) {
    // Add callbacks
    XIMU3_connection_add_receive_error_callback(connection, receive_error_callback, NULL);
    XIMU3_connection_add_statistics_callback(connection, statistics_callback, NULL);

    if (yes_or_no("Print data messages?")) {
        XIMU3_connection_add_inertial_callback(connection, inertial_callback, NULL);
        XIMU3_connection_add_magnetometer_callback(connection, magnetometer_callback, NULL);
        XIMU3_connection_add_quaternion_callback(connection, quaternion_callback, NULL);
        XIMU3_connection_add_rotation_matrix_callback(connection, rotation_matrix_callback, NULL);
        XIMU3_connection_add_euler_angles_callback(connection, euler_angles_callback, NULL);
        XIMU3_connection_add_linear_acceleration_callback(connection, linear_acceleration_callback, NULL);
        XIMU3_connection_add_earth_acceleration_callback(connection, earth_acceleration_callback, NULL);
        XIMU3_connection_add_ahrs_status_callback(connection, ahrs_status_callback, NULL);
        XIMU3_connection_add_high_g_accelerometer_callback(connection, high_g_accelerometer_callback, NULL);
        XIMU3_connection_add_temperature_callback(connection, temperature_callback, NULL);
        XIMU3_connection_add_battery_callback(connection, battery_callback, NULL);
        XIMU3_connection_add_rssi_callback(connection, rssi_callback, NULL);
        XIMU3_connection_add_serial_accessory_callback(connection, serial_accessory_callback, NULL);
        XIMU3_connection_add_notification_callback(connection, notification_callback, NULL);
        XIMU3_connection_add_error_callback(connection, error_callback, NULL);
    }

    XIMU3_connection_add_end_of_file_callback(connection, end_of_file_callback, NULL);

    // Open connection
    const XIMU3_Result result = XIMU3_connection_open(connection);

    if (result != XIMU3_ResultOk) {
        printf("Unable to open %s. %s.\n", XIMU3_connection_get_info_string(connection), XIMU3_result_to_string(result));
        XIMU3_connection_free(connection);
        return;
    }

    // Send command to strobe LED
    XIMU3_connection_send_command(connection, "{\"strobe\":null}", XIMU3_DEFAULT_RETRIES, XIMU3_DEFAULT_TIMEOUT);

    // Close connection
    sleep(60);

    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

void receive_error_callback(const XIMU3_ReceiveError error, void *context) {
    printf("%s\n", XIMU3_receive_error_to_string(error));
}

void statistics_callback(const XIMU3_Statistics statistics, void *context) {
    printf(TIMESTAMP_FORMAT UINT64_FORMAT " bytes" UINT32_FORMAT " bytes/s" UINT64_FORMAT " messages" UINT32_FORMAT " messages/s" UINT64_FORMAT " errors" UINT32_FORMAT " errors/s\n",
           statistics.timestamp,
           statistics.data_total,
           statistics.data_rate,
           statistics.message_total,
           statistics.message_rate,
           statistics.error_total,
           statistics.error_rate);
    // printf("%s\n", XIMU3_statistics_to_string(statistics)); // alternative to above
}

void inertial_callback(const XIMU3_InertialMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " deg/s" FLOAT_FORMAT " deg/s" FLOAT_FORMAT " deg/s" FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
           message.timestamp,
           message.gyroscope_x,
           message.gyroscope_y,
           message.gyroscope_z,
           message.accelerometer_x,
           message.accelerometer_y,
           message.accelerometer_z);
    // printf("%s\n", XIMU3_inertial_message_to_string(message)); // alternative to above
}

void magnetometer_callback(const XIMU3_MagnetometerMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " a.u." FLOAT_FORMAT " a.u." FLOAT_FORMAT " a.u.\n",
           message.timestamp,
           message.x,
           message.y,
           message.z);
    // printf("%s\n", XIMU3_magnetometer_message_to_string(message)); // alternative to above
}

void quaternion_callback(const XIMU3_QuaternionMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT "\n",
           message.timestamp,
           message.w,
           message.x,
           message.y,
           message.z);
    // printf("%s\n", XIMU3_quaternion_message_to_string(message)); // alternative to above
    printf("%s\n", XIMU3_euler_angles_message_to_string(XIMU3_quaternion_message_to_euler_angles_message(message)));
}

void rotation_matrix_callback(const XIMU3_RotationMatrixMessage message, void *context) {
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
    // printf("%s\n", XIMU3_rotation_matrix_message_to_string(message)); // alternative to above
    printf("%s\n", XIMU3_euler_angles_message_to_string(XIMU3_rotation_matrix_message_to_euler_angles_message(message)));
}

void euler_angles_callback(const XIMU3_EulerAnglesMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " deg" FLOAT_FORMAT " deg" FLOAT_FORMAT " deg\n",
           message.timestamp,
           message.roll,
           message.pitch,
           message.yaw);
    // printf("%s\n", XIMU3_euler_angles_message_to_string(message)); // alternative to above
    printf("%s\n", XIMU3_quaternion_message_to_string(XIMU3_euler_angles_message_to_quaternion_message(message)));
}

void linear_acceleration_callback(const XIMU3_LinearAccelerationMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
           message.timestamp,
           message.quaternion_w,
           message.quaternion_x,
           message.quaternion_y,
           message.quaternion_z,
           message.acceleration_x,
           message.acceleration_y,
           message.acceleration_z);
    // printf("%s\n", XIMU3_linear_acceleration_message_to_string(message)); // alternative to above
    printf("%s\n", XIMU3_euler_angles_message_to_string(XIMU3_linear_acceleration_message_to_euler_angles_message(message)));
}

void earth_acceleration_callback(const XIMU3_EarthAccelerationMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
           message.timestamp,
           message.quaternion_w,
           message.quaternion_x,
           message.quaternion_y,
           message.quaternion_z,
           message.acceleration_x,
           message.acceleration_y,
           message.acceleration_z);
    // printf("%s\n", XIMU3_earth_acceleration_message_to_string(message)); // alternative to above
    printf("%s\n", XIMU3_euler_angles_message_to_string(XIMU3_earth_acceleration_message_to_euler_angles_message(message)));
}

void ahrs_status_callback(const XIMU3_AhrsStatusMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT "\n",
           message.timestamp,
           message.initialising,
           message.angular_rate_recovery,
           message.acceleration_recovery,
           message.magnetic_recovery);
    // printf("%s\n", XIMU3_ahrs_status_message_to_string(message)); // alternative to above
}

void high_g_accelerometer_callback(const XIMU3_HighGAccelerometerMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
           message.timestamp,
           message.x,
           message.y,
           message.z);
    // printf("%s\n", XIMU3_high_g_accelerometer_message_to_string(message)); // alternative to above
}

void temperature_callback(const XIMU3_TemperatureMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " degC\n",
           message.timestamp,
           message.temperature);
    // printf("%s\n", XIMU3_temperature_message_to_string(message)); // alternative to above
}

void battery_callback(const XIMU3_BatteryMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " %%" FLOAT_FORMAT " V" FLOAT_FORMAT " (%s)\n",
           message.timestamp,
           message.percentage,
           message.voltage,
           message.charging_status,
           XIMU3_charging_status_to_string(XIMU3_charging_status_from_float(message.charging_status)));
    // printf("%s (%s)\n", XIMU3_battery_message_to_string(message), XIMU3_charging_status_to_string(XIMU3_charging_status_from_float(message.charging_status))); // alternative to above
}

void rssi_callback(const XIMU3_RssiMessage message, void *context) {
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " %%" FLOAT_FORMAT " dBm\n",
           message.timestamp,
           message.percentage,
           message.power);
    // printf("%s\n", XIMU3_rssi_message_to_string(message)); // alternative to above
}

void serial_accessory_callback(const XIMU3_SerialAccessoryMessage message, void *context) {
    printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
           message.timestamp,
           message.char_array);
    // printf("%s\n", XIMU3_serial_accessory_message_to_string(message)); // alternative to above
}

void notification_callback(const XIMU3_NotificationMessage message, void *context) {
    printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
           message.timestamp,
           message.char_array);
    // printf("%s\n", XIMU3_notification_message_to_string(message)); // alternative to above
}

void error_callback(const XIMU3_ErrorMessage message, void *context) {
    printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
           message.timestamp,
           message.char_array);
    // printf("%s\n", XIMU3_error_message_to_string(message)); // alternative to above
}

void end_of_file_callback(void *context) {
    printf("End of file\n");
}
