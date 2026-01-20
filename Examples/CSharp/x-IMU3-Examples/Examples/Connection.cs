namespace Ximu3Examples
{
    class Connection
    {
        protected static void Run(Ximu3.ConnectionConfig config)
        {
            // Create connection
            Ximu3.Connection connection = new(config);

            connection.AddReceiveErrorCallback(ReceiveErrorCallback);
            connection.AddStatisticsCallback(StatisticsCallback);

            if (Helpers.YesOrNo("Print data messages?"))
            {
                connection.AddInertialCallback(InertialCallback);
                connection.AddMagnetometerCallback(MagnetometerCallback);
                connection.AddQuaternionCallback(QuaternionCallback);
                connection.AddRotationMatrixCallback(RotationMatrixCallback);
                connection.AddEulerAnglesCallback(EulerAnglesCallback);
                connection.AddLinearAccelerationCallback(LinearAccelerationCallback);
                connection.AddEarthAccelerationCallback(EarthAccelerationCallback);
                connection.AddAhrsStatusCallback(AhrsStatusCallback);
                connection.AddHighGAccelerometerCallback(HighGAccelerometerCallback);
                connection.AddTemperatureCallback(TemperatureCallback);
                connection.AddBatteryCallback(BatteryCallback);
                connection.AddRssiCallback(RssiCallback);
                connection.AddSerialAccessoryCallback(SerialAccessoryCallback);
                connection.AddNotificationCallback(NotificationCallback);
                connection.AddErrorCallback(ErrorCallback);
            }
            connection.AddEndOfFileCallback(EndOfFileCallback);

            // Open connection
            Ximu3.CApi.XIMU3_Result result = connection.Open();

            if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open " + connection.GetConfig() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                return;
            }

            // Send command to strobe LED
            connection.SendCommand("{\"strobe\":null}");

            // Close connection
            System.Threading.Thread.Sleep(60000);

            connection.Close();
        }

        private static string TimestampFormat(UInt64 timestamp)
        {
            return $"{timestamp,8} us";
        }

        private static string IntFormat(uint value)
        {
            return $" {value,8}";
        }

        private static string IntFormat(ulong value)
        {
            return $" {value,8}";
        }

        private static string FloatFormat(float value)
        {
            return $" {value.ToString("###0.000", System.Globalization.CultureInfo.InvariantCulture),8}";
        }

        private static string StringFormat(string value)
        {
            return $@" ""{value}""";
        }

        private static void ReceiveErrorCallback(Ximu3.CApi.XIMU3_ReceiveError error)
        {
            Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_receive_error_to_string(error)));
        }

        private static void StatisticsCallback(Ximu3.CApi.XIMU3_Statistics statistics)
        {
            Console.WriteLine(TimestampFormat(statistics.timestamp) +
                              IntFormat(statistics.data_total) + " bytes" +
                              IntFormat(statistics.data_rate) + " bytes/s" +
                              IntFormat(statistics.message_total) + " messages" +
                              IntFormat(statistics.message_rate) + " messages/s" +
                              IntFormat(statistics.error_total) + " errors" +
                              IntFormat(statistics.error_rate) + " errors/s");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_statistics_to_string(statistics))); // alternative to above
        }

        private static void InertialCallback(Ximu3.CApi.XIMU3_InertialMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.gyroscope_x) + " deg/s" +
                              FloatFormat(message.gyroscope_y) + " deg/s" +
                              FloatFormat(message.gyroscope_z) + " deg/s" +
                              FloatFormat(message.accelerometer_x) + " g" +
                              FloatFormat(message.accelerometer_y) + " g" +
                              FloatFormat(message.accelerometer_z) + " g");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_inertial_message_to_string(message))); // alternative to above
        }

        private static void MagnetometerCallback(Ximu3.CApi.XIMU3_MagnetometerMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.x) + " a.u." +
                              FloatFormat(message.y) + " a.u." +
                              FloatFormat(message.z) + " a.u.");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_magnetometer_message_to_string(message))); // alternative to above
        }

        private static void QuaternionCallback(Ximu3.CApi.XIMU3_QuaternionMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.w) +
                              FloatFormat(message.x) +
                              FloatFormat(message.y) +
                              FloatFormat(message.z));
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_quaternion_message_to_string(message))); // alternative to above
            Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_euler_angles_message_to_string(Ximu3.CApi.XIMU3_quaternion_message_to_euler_angles_message(message))));
        }

        private static void RotationMatrixCallback(Ximu3.CApi.XIMU3_RotationMatrixMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.xx) +
                              FloatFormat(message.xy) +
                              FloatFormat(message.xz) +
                              FloatFormat(message.yx) +
                              FloatFormat(message.yy) +
                              FloatFormat(message.yz) +
                              FloatFormat(message.zx) +
                              FloatFormat(message.zy) +
                              FloatFormat(message.zz));
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_rotation_matrix_message_to_string(message))); // alternative to above
            Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_euler_angles_message_to_string(Ximu3.CApi.XIMU3_rotation_matrix_message_to_euler_angles_message(message))));
        }

        private static void EulerAnglesCallback(Ximu3.CApi.XIMU3_EulerAnglesMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.roll) + " deg" +
                              FloatFormat(message.pitch) + " deg" +
                              FloatFormat(message.yaw) + " deg");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_euler_angles_message_to_string(message))); // alternative to above
        }

        private static void LinearAccelerationCallback(Ximu3.CApi.XIMU3_LinearAccelerationMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.quaternion_w) +
                              FloatFormat(message.quaternion_x) +
                              FloatFormat(message.quaternion_y) +
                              FloatFormat(message.quaternion_z) +
                              FloatFormat(message.acceleration_x) + " g" +
                              FloatFormat(message.acceleration_y) + " g" +
                              FloatFormat(message.acceleration_z) + " g");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_linear_acceleration_message_to_string(message))); // alternative to above
            Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_euler_angles_message_to_string(Ximu3.CApi.XIMU3_linear_acceleration_message_to_euler_angles_message(message))));
        }

        private static void EarthAccelerationCallback(Ximu3.CApi.XIMU3_EarthAccelerationMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.quaternion_w) +
                              FloatFormat(message.quaternion_x) +
                              FloatFormat(message.quaternion_y) +
                              FloatFormat(message.quaternion_z) +
                              FloatFormat(message.acceleration_x) + " g" +
                              FloatFormat(message.acceleration_y) + " g" +
                              FloatFormat(message.acceleration_z) + " g");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_earth_acceleration_message_to_string(message))); // alternative to above
            Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_euler_angles_message_to_string(Ximu3.CApi.XIMU3_earth_acceleration_message_to_euler_angles_message(message))));
        }

        private static void AhrsStatusCallback(Ximu3.CApi.XIMU3_AhrsStatusMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.initialising) +
                              FloatFormat(message.angular_rate_recovery) +
                              FloatFormat(message.acceleration_recovery) +
                              FloatFormat(message.magnetic_recovery));
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_ahrs_status_message_to_string(message))); // alternative to above
        }

        private static void HighGAccelerometerCallback(Ximu3.CApi.XIMU3_HighGAccelerometerMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.x) + " g" +
                              FloatFormat(message.y) + " g" +
                              FloatFormat(message.z) + " g");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_high_g_accelerometer_message_to_string(message))); // alternative to above
        }

        private static void TemperatureCallback(Ximu3.CApi.XIMU3_TemperatureMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.temperature) + " degC");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_temperature_message_to_string(message))); // alternative to above
        }

        private static void BatteryCallback(Ximu3.CApi.XIMU3_BatteryMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.percentage) + " %" +
                              FloatFormat(message.voltage) + " V" +
                              FloatFormat(message.charging_status) + $" ({Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_charging_status_to_string(Ximu3.CApi.XIMU3_charging_status_from_float(message.charging_status)))})");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_battery_message_to_string(message)) + $" ({Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_charging_status_to_string(Ximu3.CApi.XIMU3_charging_status_from_float(message.charging_status)))})"); // alternative to above
        }

        private static void RssiCallback(Ximu3.CApi.XIMU3_RssiMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) +
                              FloatFormat(message.percentage) + " %" +
                              FloatFormat(message.power) + " dBm");
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_rssi_message_to_string(message))); // alternative to above
        }

        private static void SerialAccessoryCallback(Ximu3.CApi.XIMU3_SerialAccessoryMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) + StringFormat(Ximu3.Helpers.ToString(message.char_array)));
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_serial_accessory_message_to_string(message))); // alternative to above
        }

        private static void NotificationCallback(Ximu3.CApi.XIMU3_NotificationMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) + StringFormat(Ximu3.Helpers.ToString(message.char_array)));
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_notification_message_to_string(message))); // alternative to above
        }

        private static void ErrorCallback(Ximu3.CApi.XIMU3_ErrorMessage message)
        {
            Console.WriteLine(TimestampFormat(message.timestamp) + StringFormat(Ximu3.Helpers.ToString(message.char_array)));
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_error_message_to_string(message))); // alternative to above
        }

        private static void EndOfFileCallback()
        {
            Console.WriteLine("End of file");
        }
    }
}
