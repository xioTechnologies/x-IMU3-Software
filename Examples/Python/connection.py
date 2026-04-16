import time

import helpers
import ximu3


def timestamp_format(timestamp: int) -> str:
    return f"{timestamp:8d} us"


def int_format(value: int) -> str:
    return f" {value:8d}"


def float_format(value: float) -> str:
    return f" {value:8.3f}"


def string_format(string: str) -> str:
    return f' "{string}"'


def receive_error_callback(error: int) -> None:
    print(ximu3.receive_error_to_string(error))


def statistics_callback(statistics: ximu3.Statistics) -> None:
    print(
        "".join(
            [
                timestamp_format(statistics.timestamp),
                int_format(statistics.data_total) + " bytes",
                int_format(statistics.data_rate) + " bytes/s",
                int_format(statistics.message_total) + " messages",
                int_format(statistics.message_rate) + " messages/s",
                int_format(statistics.error_total) + " errors",
                int_format(statistics.error_rate) + " errors/s",
            ]
        )
    )
    # print(statistics)  # alternative to above


def inertial_callback(message: ximu3.InertialMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.gyroscope_x) + " deg/s",
                float_format(message.gyroscope_y) + " deg/s",
                float_format(message.gyroscope_z) + " deg/s",
                float_format(message.accelerometer_x) + " g",
                float_format(message.accelerometer_y) + " g",
                float_format(message.accelerometer_z) + " g",
            ]
        )
    )
    # print(message)  # alternative to above


def magnetometer_callback(message: ximu3.MagnetometerMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.x) + " a.u.",
                float_format(message.y) + " a.u.",
                float_format(message.z) + " a.u.",
            ]
        )
    )
    # print(message)  # alternative to above


def high_g_accelerometer_callback(message: ximu3.HighGAccelerometerMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.x) + " g",
                float_format(message.y) + " g",
                float_format(message.z) + " g",
            ]
        )
    )
    # print(message)  # alternative to above


def quaternion_callback(message: ximu3.QuaternionMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.w),
                float_format(message.x),
                float_format(message.y),
                float_format(message.z),
            ]
        )
    )
    # print(message)  # alternative to above
    print(message.to_euler_angles_message())


def rotation_matrix_callback(message: ximu3.RotationMatrixMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.xx),
                float_format(message.xy),
                float_format(message.xz),
                float_format(message.yx),
                float_format(message.yy),
                float_format(message.yz),
                float_format(message.zx),
                float_format(message.zy),
                float_format(message.zz),
            ]
        )
    )
    # print(message)  # alternative to above
    print(message.to_euler_angles_message())


def euler_angles_callback(message: ximu3.EulerAnglesMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.roll) + " deg",
                float_format(message.pitch) + " deg",
                float_format(message.yaw) + " deg",
            ]
        )
    )
    # print(message)  # alternative to above
    print(message.to_quaternion_message())


def linear_acceleration_callback(message: ximu3.LinearAccelerationMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.quaternion_w),
                float_format(message.quaternion_x),
                float_format(message.quaternion_y),
                float_format(message.quaternion_z),
                float_format(message.acceleration_x) + " g",
                float_format(message.acceleration_y) + " g",
                float_format(message.acceleration_z) + " g",
            ]
        )
    )
    # print(message)  # alternative to above
    print(message.to_euler_angles_message())


def earth_acceleration_callback(message: ximu3.EarthAccelerationMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.quaternion_w),
                float_format(message.quaternion_x),
                float_format(message.quaternion_y),
                float_format(message.quaternion_z),
                float_format(message.acceleration_x) + " g",
                float_format(message.acceleration_y) + " g",
                float_format(message.acceleration_z) + " g",
            ]
        )
    )
    # print(message)  # alternative to above
    print(message.to_euler_angles_message())


def ahrs_status_callback(message: ximu3.AhrsStatusMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.initialising),
                float_format(message.angular_rate_recovery),
                float_format(message.acceleration_recovery),
                float_format(message.magnetic_recovery),
            ]
        )
    )
    # print(message)  # alternative to above


def serial_accessory_callback(message: ximu3.SerialAccessoryMessage) -> None:
    print(timestamp_format(message.timestamp) + string_format(message.string))
    # print(message)  # alternative to above


def sync_callback(message: ximu3.SyncMessage) -> None:
    print(timestamp_format(message.timestamp) + float_format(message.edge))
    # print(message)  # alternative to above


def ltc_callback(message: ximu3.LtcMessage) -> None:
    print(timestamp_format(message.timestamp) + string_format(message.string))
    # print(message)  # alternative to above


def temperature_callback(message: ximu3.TemperatureMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.temperature) + " degC",
            ]
        )
    )
    # print(message)  # alternative to above


def battery_callback(message: ximu3.BatteryMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.percentage) + " %",
                float_format(message.voltage) + " V",
                float_format(message.charging_status) + f" ({ximu3.charging_status_to_string(ximu3.charging_status_from_float(message.charging_status))})",
            ]
        )
    )
    # print(message)  # alternative to above


def rssi_callback(message: ximu3.RssiMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.percentage) + " %",
                float_format(message.power) + " dBm",
            ]
        )
    )
    # print(message)  # alternative to above


def button_callback(message: ximu3.ButtonMessage) -> None:
    print(timestamp_format(message.timestamp) + float_format(message.state))
    # print(message)  # alternative to above


def notification_callback(message: ximu3.NotificationMessage) -> None:
    print(timestamp_format(message.timestamp) + string_format(message.string))
    # print(message)  # alternative to above


def error_callback(message: ximu3.ErrorMessage) -> None:
    print(timestamp_format(message.timestamp) + string_format(message.string))
    # print(message)  # alternative to above


def end_of_file_callback() -> None:
    print("End of file")


def run(config: ximu3.ConnectionConfig) -> None:
    connection = ximu3.Connection(config)

    connection.open()

    connection.send_command('{"strobe":null}')  # send command to strobe LED

    if helpers.yes_or_no("Use callbacks (else poll)?"):
        connection.add_receive_error_callback(receive_error_callback)
        connection.add_statistics_callback(statistics_callback)
        connection.add_inertial_callback(inertial_callback)
        connection.add_magnetometer_callback(magnetometer_callback)
        connection.add_high_g_accelerometer_callback(high_g_accelerometer_callback)
        connection.add_quaternion_callback(quaternion_callback)
        connection.add_rotation_matrix_callback(rotation_matrix_callback)
        connection.add_euler_angles_callback(euler_angles_callback)
        connection.add_linear_acceleration_callback(linear_acceleration_callback)
        connection.add_earth_acceleration_callback(earth_acceleration_callback)
        connection.add_ahrs_status_callback(ahrs_status_callback)
        connection.add_serial_accessory_callback(serial_accessory_callback)
        connection.add_sync_callback(sync_callback)
        connection.add_ltc_callback(ltc_callback)
        connection.add_temperature_callback(temperature_callback)
        connection.add_battery_callback(battery_callback)
        connection.add_rssi_callback(rssi_callback)
        connection.add_button_callback(button_callback)
        connection.add_notification_callback(notification_callback)
        connection.add_error_callback(error_callback)
        connection.add_end_of_file_callback(end_of_file_callback)

        time.sleep(60)
    else:
        for _ in range(60000):
            print(connection.get_statistics())
            print(connection.get_inertial_message())
            print(connection.get_magnetometer_message())
            print(connection.get_high_g_accelerometer_message())
            print(connection.get_quaternion_message())
            print(connection.get_rotation_matrix_message())
            print(connection.get_euler_angles_message())
            print(connection.get_linear_acceleration_message())
            print(connection.get_earth_acceleration_message())
            print(connection.get_ahrs_status_message())
            print(connection.get_serial_accessory_message())
            print(connection.get_sync_message())
            print(connection.get_ltc_message())
            print(connection.get_temperature_message())
            print(connection.get_battery_message())
            print(connection.get_rssi_message())
            print(connection.get_button_message())
            print(connection.get_notification_message())
            print(connection.get_error_message())

            time.sleep(0.001)

    connection.close()
