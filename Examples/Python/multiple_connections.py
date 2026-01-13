import time
from typing import Any

import ximu3


class Connection:
    def __init__(self, connection_info: ximu3.UsbConnectionInfo | ximu3.SerialConnectionInfo | ximu3.TcpConnectionInfo | ximu3.UdpConnectionInfo | ximu3.BluetoothConnectionInfo | ximu3.FileConnectionInfo | ximu3.MuxConnectionInfo) -> None:
        self.__connection = ximu3.Connection(connection_info)

        result = self.__connection.open()

        if result != ximu3.RESULT_OK:
            raise Exception(f"Unable to open {connection.get_info().to_string()}. {ximu3.result_to_string(result)}.")

        ping_response = self.__connection.ping()  # send ping so that device starts sending to computer's IP address

        if ping_response is None:
            raise Exception(f"Ping failed for {connection_info.to_string()}")

        self.__prefix = f"{ping_response.device_name} {ping_response.serial_number} "

        self.__connection.add_inertial_callback(self.__inertial_callback)
        self.__connection.add_magnetometer_callback(self.__magnetometer_callback)
        self.__connection.add_quaternion_callback(self.__quaternion_callback)
        self.__connection.add_rotation_matrix_callback(self.__rotation_matrix_callback)
        self.__connection.add_euler_angles_callback(self.__euler_angles_callback)
        self.__connection.add_linear_acceleration_callback(self.__linear_acceleration_callback)
        self.__connection.add_earth_acceleration_callback(self.__earth_acceleration_callback)
        self.__connection.add_ahrs_status_callback(self.__ahrs_status_callback)
        self.__connection.add_high_g_accelerometer_callback(self.__high_g_accelerometer_callback)
        self.__connection.add_temperature_callback(self.__temperature_callback)
        self.__connection.add_battery_callback(self.__battery_callback)
        self.__connection.add_rssi_callback(self.__rssi_callback)
        self.__connection.add_serial_accessory_callback(self.__serial_accessory_callback)
        self.__connection.add_notification_callback(self.__notification_callback)
        self.__connection.add_error_callback(self.__error_callback)

    def close(self) -> None:
        self.__connection.close()

    def send_command(self, key: str, value: Any = None) -> None:
        if value is None:
            value = "null"
        elif type(value) is bool:
            value = str(value).lower()
        elif type(value) is str:
            value = f'"{value}"'
        else:
            value = str(value)

        command = f'{{"{key}":{value}}}'

        response = self.__connection.send_command(command)

        if response is None:
            raise Exception(f"No response. {command} sent to {self.__connection.get_info().to_string()}")

        if response.error:
            raise Exception(f"{response.error}. {command} sent to {self.__connection.get_info().to_string()}")

        print(self.__prefix + f"{response.key} : {response.value}")

    def __inertial_callback(self, message: ximu3.InertialMessage) -> None:
        print(self.__prefix + message.to_string())

    def __magnetometer_callback(self, message: ximu3.MagnetometerMessage) -> None:
        print(self.__prefix + message.to_string())

    def __quaternion_callback(self, message: ximu3.QuaternionMessage) -> None:
        print(self.__prefix + message.to_string())

    def __rotation_matrix_callback(self, message: ximu3.RotationMatrixMessage) -> None:
        print(self.__prefix + message.to_string())

    def __euler_angles_callback(self, message: ximu3.EulerAnglesMessage) -> None:
        print(self.__prefix + message.to_string())

    def __linear_acceleration_callback(self, message: ximu3.LinearAccelerationMessage) -> None:
        print(self.__prefix + message.to_string())

    def __earth_acceleration_callback(self, message: ximu3.EarthAccelerationMessage) -> None:
        print(self.__prefix + message.to_string())

    def __ahrs_status_callback(self, message: ximu3.AhrsStatusMessage) -> None:
        print(self.__prefix + message.to_string())

    def __high_g_accelerometer_callback(self, message: ximu3.HighGAccelerometerMessage) -> None:
        print(self.__prefix + message.to_string())

    def __temperature_callback(self, message: ximu3.TemperatureMessage) -> None:
        print(self.__prefix + message.to_string())

    def __battery_callback(self, message: ximu3.BatteryMessage) -> None:
        print(self.__prefix + message.to_string())

    def __rssi_callback(self, message: ximu3.RssiMessage) -> None:
        print(self.__prefix + message.to_string())

    def __serial_accessory_callback(self, message: ximu3.SerialAccessoryMessage) -> None:
        print(self.__prefix + message.to_string())

    def __notification_callback(self, message: ximu3.NotificationMessage) -> None:
        print(self.__prefix + message.to_string())

    def __error_callback(self, message: ximu3.ErrorMessage) -> None:
        print(self.__prefix + message.to_string())


connections = [Connection(m.to_udp_connection_info()) for m in ximu3.NetworkAnnouncement().get_messages_after_short_delay()]

if not connections:
    raise Exception("No UDP connections available")

for connection in connections:
    connection.send_command("strobe")  # example command with null value
    connection.send_command("note", "Hello World!")  # example command with string value
    connection.send_command("udp_data_messages_enabled", True)  # example command with true/false value
    connection.send_command("inertial_message_rate_divisor", 8)  # example command with number value

time.sleep(60)

for connection in connections:
    connection.close()
