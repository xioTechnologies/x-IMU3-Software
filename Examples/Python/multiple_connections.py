import time
import ximu3


class Connection:
    def __init__(self, connection_info):
        self.__connection = ximu3.Connection(connection_info)

        if self.__connection.open() != ximu3.RESULT_OK:
            raise Exception(f"Unable to open {connection_info.to_string()}")

        ping_response = self.__connection.ping()  # send ping so that device starts sending to computer's IP address

        if ping_response.result != ximu3.RESULT_OK:
            raise Exception(f"No ping response for {connection_info.to_string()}")

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

    def close(self):
        self.__connection.close()

    def send_command(self, key, value=None):
        if value is None:
            value = "null"
        elif type(value) is bool:
            value = str(value).lower()
        elif type(value) is str:
            value = f'"{value}"'
        else:
            value = str(value)

        command = f'{{"{key}":{value}}}'

        responses = self.__connection.send_commands([command], 2, 500)

        if not responses:
            raise Exception(f"No response. {command} sent to {self.__connection.get_info().to_string()}")

        response = ximu3.CommandMessage.parse(responses[0])

        if response.error:
            raise Exception(f"{response.error}. {command} sent to {self.__connection.get_info().to_string()}")

        print(self.__prefix + f"{response.key} : {response.value}")

    def __inertial_callback(self, message):
        print(self.__prefix + message.to_string())

    def __magnetometer_callback(self, message):
        print(self.__prefix + message.to_string())

    def __quaternion_callback(self, message):
        print(self.__prefix + message.to_string())

    def __rotation_matrix_callback(self, message):
        print(self.__prefix + message.to_string())

    def __euler_angles_callback(self, message):
        print(self.__prefix + message.to_string())

    def __linear_acceleration_callback(self, message):
        print(self.__prefix + message.to_string())

    def __earth_acceleration_callback(self, message):
        print(self.__prefix + message.to_string())

    def __ahrs_status_callback(self, message):
        print(self.__prefix + message.to_string())

    def __high_g_accelerometer_callback(self, message):
        print(self.__prefix + message.to_string())

    def __temperature_callback(self, message):
        print(self.__prefix + message.to_string())

    def __battery_callback(self, message):
        print(self.__prefix + message.to_string())

    def __rssi_callback(self, message):
        print(self.__prefix + message.to_string())

    def __serial_accessory_callback(self, message):
        print(self.__prefix + message.to_string())

    def __notification_callback(self, message):
        print(self.__prefix + message.to_string())

    def __error_callback(self, message):
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
