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


def decode_error_callback(error: int) -> None:
    print(ximu3.decode_error_to_string(error))


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
    # print(statistics.to_string())  # alternative to above


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
    # print(message.to_string())  # alternative to above


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
    # print(message.to_string())  # alternative to above


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
    # print(message.to_string())  # alternative to above
    print(message.to_euler_angles_message().to_string())


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
    # print(message.to_string())  # alternative to above
    print(message.to_euler_angles_message().to_string())


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
    # print(message.to_string())  # alternative to above
    print(message.to_quaternion_message().to_string())


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
    # print(message.to_string())  # alternative to above
    print(message.to_euler_angles_message().to_string())


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
    # print(message.to_string())  # alternative to above
    print(message.to_euler_angles_message().to_string())


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
    # print(message.to_string())  # alternative to above


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
    # print(message.to_string())  # alternative to above


def temperature_callback(message: ximu3.TemperatureMessage) -> None:
    print(
        "".join(
            [
                timestamp_format(message.timestamp),
                float_format(message.temperature) + " degC",
            ]
        )
    )
    # print(message.to_string())  # alternative to above


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
    # print(message.to_string() + f" ({ximu3.charging_status_to_string(ximu3.charging_status_from_float(message.charging_status))})")  # alternative to above


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
    # print(message.to_string())  # alternative to above


def serial_accessory_callback(message: ximu3.SerialAccessoryMessage) -> None:
    print(timestamp_format(message.timestamp) + string_format(message.string))
    # print(message.to_string())  # alternative to above


def notification_callback(message: ximu3.NotificationMessage) -> None:
    print(timestamp_format(message.timestamp) + string_format(message.string))
    # print(message.to_string())  # alternative to above


def error_callback(message: ximu3.ErrorMessage) -> None:
    print(timestamp_format(message.timestamp) + string_format(message.string))
    # print(message.to_string())  # alternative to above


def end_of_file_callback() -> None:
    print("End of file")


def run(connection_info: ximu3.UsbConnectionInfo | ximu3.SerialConnectionInfo | ximu3.TcpConnectionInfo | ximu3.UdpConnectionInfo | ximu3.BluetoothConnectionInfo | ximu3.FileConnectionInfo | ximu3.MuxConnectionInfo) -> None:
    connection = ximu3.Connection(connection_info)

    connection.add_decode_error_callback(decode_error_callback)
    connection.add_statistics_callback(statistics_callback)

    if helpers.yes_or_no("Print data messages?"):
        connection.add_inertial_callback(inertial_callback)
        connection.add_magnetometer_callback(magnetometer_callback)
        connection.add_quaternion_callback(quaternion_callback)
        connection.add_rotation_matrix_callback(rotation_matrix_callback)
        connection.add_euler_angles_callback(euler_angles_callback)
        connection.add_linear_acceleration_callback(linear_acceleration_callback)
        connection.add_earth_acceleration_callback(earth_acceleration_callback)
        connection.add_ahrs_status_callback(ahrs_status_callback)
        connection.add_high_g_accelerometer_callback(high_g_accelerometer_callback)
        connection.add_temperature_callback(temperature_callback)
        connection.add_battery_callback(battery_callback)
        connection.add_rssi_callback(rssi_callback)
        connection.add_serial_accessory_callback(serial_accessory_callback)
        connection.add_notification_callback(notification_callback)
        connection.add_error_callback(error_callback)

    connection.add_end_of_file_callback(end_of_file_callback)

    result = connection.open()

    if result != ximu3.RESULT_OK:
        raise Exception(f"Unable to open {connection_info.to_string()}. {ximu3.result_to_string(result)}.")

    connection.send_commands(['{"strobe":null}'], 2, 500)  # send command to strobe LED

    time.sleep(60)

    connection.close()
