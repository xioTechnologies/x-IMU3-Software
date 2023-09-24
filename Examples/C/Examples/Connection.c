#include "../Helpers.h"
#include "Connection.h"
#include <inttypes.h> // PRIu64
#include <stdio.h>

#define TIMESTAMP_FORMAT "%8" PRIu64 " us"
#define UINT32_FORMAT " %8" PRIu32
#define UINT64_FORMAT " %8" PRIu64
#define FLOAT_FORMAT " %8.3f"
#define STRING_FORMAT " \"%s\""

void DecodeErrorCallback(const XIMU3_DecodeError error, void* context);

void StatisticsCallback(const XIMU3_Statistics statistics, void* context);

void InertialCallback(const XIMU3_InertialMessage message, void* context);

void MagnetometerCallback(const XIMU3_MagnetometerMessage message, void* context);

void QuaternionCallback(const XIMU3_QuaternionMessage message, void* context);

void RotationMatrixCallback(const XIMU3_RotationMatrixMessage message, void* context);

void EulerAnglesCallback(const XIMU3_EulerAnglesMessage message, void* context);

void LinearAccelerationCallback(const XIMU3_LinearAccelerationMessage message, void* context);

void EarthAccelerationCallback(const XIMU3_EarthAccelerationMessage message, void* context);

void AhrsStatusCallback(const XIMU3_AhrsStatusMessage message, void* context);

void HighGAccelerometerCallback(const XIMU3_HighGAccelerometerMessage message, void* context);

void TemperatureCallback(const XIMU3_TemperatureMessage message, void* context);

void BatteryCallback(const XIMU3_BatteryMessage message, void* context);

void RssiCallback(const XIMU3_RssiMessage message, void* context);

void SerialAccessoryCallback(const XIMU3_SerialAccessoryMessage message, void* context);

void NotificationCallback(const XIMU3_NotificationMessage message, void* context);

void ErrorCallback(const XIMU3_ErrorMessage message, void* context);

void Run(XIMU3_Connection* const connection, const char* const connectionInfoString)
{
    // Add callbacks
    XIMU3_connection_add_decode_error_callback(connection, DecodeErrorCallback, NULL);
    XIMU3_connection_add_statistics_callback(connection, StatisticsCallback, NULL);

    if (AskQuestion("Print data messages?"))
    {
        XIMU3_connection_add_inertial_callback(connection, InertialCallback, NULL);
        XIMU3_connection_add_magnetometer_callback(connection, MagnetometerCallback, NULL);
        XIMU3_connection_add_quaternion_callback(connection, QuaternionCallback, NULL);
        XIMU3_connection_add_rotation_matrix_callback(connection, RotationMatrixCallback, NULL);
        XIMU3_connection_add_euler_angles_callback(connection, EulerAnglesCallback, NULL);
        XIMU3_connection_add_linear_acceleration_callback(connection, LinearAccelerationCallback, NULL);
        XIMU3_connection_add_earth_acceleration_callback(connection, EarthAccelerationCallback, NULL);
        XIMU3_connection_add_ahrs_status_callback(connection, AhrsStatusCallback, NULL);
        XIMU3_connection_add_high_g_accelerometer_callback(connection, HighGAccelerometerCallback, NULL);
        XIMU3_connection_add_temperature_callback(connection, TemperatureCallback, NULL);
        XIMU3_connection_add_battery_callback(connection, BatteryCallback, NULL);
        XIMU3_connection_add_rssi_callback(connection, RssiCallback, NULL);
        XIMU3_connection_add_serial_accessory_callback(connection, SerialAccessoryCallback, NULL);
        XIMU3_connection_add_notification_callback(connection, NotificationCallback, NULL);
        XIMU3_connection_add_error_callback(connection, ErrorCallback, NULL);
    }

    // Open connection
    printf("Connecting to %s\n", connectionInfoString);

    if (XIMU3_connection_open(connection) != XIMU3_ResultOk)
    {
        printf("Unable to open connection\n");
        XIMU3_connection_free(connection);
        return;
    }
    printf("Connection successful\n");

    // Send command to strobe LED
    const char* const commands[] = { "{\"strobe\":null}" };
    const XIMU3_CharArrays responses = XIMU3_connection_send_commands(connection, commands, 1, 2, 500);
    XIMU3_char_arrays_free(responses);

    // Close connection
    Wait(60);
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

void DecodeErrorCallback(const XIMU3_DecodeError decodeError, void* context)
{
    printf("%s\n", XIMU3_decode_error_to_string(decodeError));
}

void StatisticsCallback(const XIMU3_Statistics statistics, void* context)
{
    printf(TIMESTAMP_FORMAT UINT64_FORMAT
           " bytes" UINT32_FORMAT
           " bytes/s" UINT64_FORMAT
           " messages" UINT32_FORMAT
           " messages/s" UINT64_FORMAT
           " errors" UINT32_FORMAT
           " errors/s\n",
           statistics.timestamp,
           statistics.data_total,
           statistics.data_rate,
           statistics.message_total,
           statistics.message_rate,
           statistics.error_total,
           statistics.error_rate);
    // printf("%s\n", XIMU3_statistics_to_string(statistics)); // alternative to above
}

void InertialCallback(const XIMU3_InertialMessage message, void* context)
{
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

void MagnetometerCallback(const XIMU3_MagnetometerMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " a.u." FLOAT_FORMAT " a.u." FLOAT_FORMAT " a.u.\n",
           message.timestamp,
           message.x,
           message.y,
           message.z);
    // printf("%s\n", XIMU3_magnetometer_message_to_string(message)); // alternative to above
}

void QuaternionCallback(const XIMU3_QuaternionMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT "\n",
           message.timestamp,
           message.w,
           message.x,
           message.y,
           message.z);
    // printf("%s\n", XIMU3_quaternion_message_to_string(message)); // alternative to above
}

void RotationMatrixCallback(const XIMU3_RotationMatrixMessage message, void* context)
{
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
}

void EulerAnglesCallback(const XIMU3_EulerAnglesMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " deg" FLOAT_FORMAT " deg" FLOAT_FORMAT " deg\n",
           message.timestamp,
           message.roll,
           message.pitch,
           message.yaw);
    // printf("%s\n", XIMU3_euler_angles_message_to_string(message)); // alternative to above
}

void LinearAccelerationCallback(const XIMU3_LinearAccelerationMessage message, void* context)
{
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
}

void EarthAccelerationCallback(const XIMU3_EarthAccelerationMessage message, void* context)
{
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
}

void AhrsStatusCallback(const XIMU3_AhrsStatusMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT FLOAT_FORMAT "\n",
           message.timestamp,
           message.initialising,
           message.angular_rate_recovery,
           message.acceleration_recovery,
           message.magnetic_recovery);
    // printf("%s\n", XIMU3_ahrs_status_message_to_string(message)); // alternative to above
}

void HighGAccelerometerCallback(const XIMU3_HighGAccelerometerMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " g" FLOAT_FORMAT " g" FLOAT_FORMAT " g\n",
           message.timestamp,
           message.x,
           message.y,
           message.z);
    // printf("%s\n", XIMU3_high_g_accelerometer_message_to_string(message)); // alternative to above
}

void TemperatureCallback(const XIMU3_TemperatureMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " degC\n",
           message.timestamp,
           message.temperature);
    // printf("%s\n", XIMU3_temperature_message_to_string(message)); // alternative to above
}

void BatteryCallback(const XIMU3_BatteryMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " %%" FLOAT_FORMAT " V" FLOAT_FORMAT "\n",
           message.timestamp,
           message.percentage,
           message.voltage,
           message.charging_status);
    // printf("%s\n", XIMU3_battery_message_to_string(message)); // alternative to above
}

void RssiCallback(const XIMU3_RssiMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT FLOAT_FORMAT " %%" FLOAT_FORMAT " dBm\n",
           message.timestamp,
           message.percentage,
           message.power);
    // printf("%s\n", XIMU3_rssi_message_to_string(message)); // alternative to above
}

void SerialAccessoryCallback(const XIMU3_SerialAccessoryMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
           message.timestamp,
           message.char_array);
    // printf("%s\n", XIMU3_serial_accessory_message_to_string(message)); // alternative to above
}

void NotificationCallback(const XIMU3_NotificationMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
           message.timestamp,
           message.char_array);
    // printf("%s\n", XIMU3_notification_message_to_string(message)); // alternative to above
}

void ErrorCallback(const XIMU3_ErrorMessage message, void* context)
{
    printf(TIMESTAMP_FORMAT STRING_FORMAT "\n",
           message.timestamp,
           message.char_array);
    // printf("%s\n", XIMU3_error_message_to_string(message)); // alternative to above
}
