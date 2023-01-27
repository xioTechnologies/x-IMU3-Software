using System;

namespace Ximu3Examples
{
    class Connection
    {
        protected void Run(Ximu3.ConnectionInfo connectionInfo)
        {
            // Create connection
            Ximu3.Connection connection = new Ximu3.Connection(connectionInfo);

            connection.DecodeErrorEvent += this.DecodeErrorEvent;
            connection.StatisticsEvent += this.StatisticsEvent;
            if (Helpers.YesOrNo("Print data messages?") == true)
            {
                connection.InertialEvent += this.InertialEvent;
                connection.MagnetometerEvent += this.MagnetometerEvent;
                connection.QuaternionEvent += this.QuaternionEvent;
                connection.RotationMatrixEvent += this.RotationMatrixEvent;
                connection.EulerAnglesEvent += this.EulerAnglesEvent;
                connection.LinearAccelerationEvent += this.LinearAccelerationEvent;
                connection.EarthAccelerationEvent += this.EarthAccelerationEvent;
                connection.HighGAccelerometerEvent += this.HighGAccelerometerEvent;
                connection.TemperatureEvent += this.TemperatureEvent;
                connection.BatteryEvent += this.BatteryEvent;
                connection.RssiEvent += this.RssiEvent;
                connection.SerialAccessoryEvent += this.SerialAccessoryEvent;
                connection.NotificationEvent += this.NotificationEvent;
                connection.ErrorEvent += this.ErrorEvent;
            }

            // Open connection
            Console.WriteLine("Connecting to " + connectionInfo);
            if (connection.Open() != Ximu3.Result.Ok)
            {
                Console.WriteLine("Unable to open connection");
                return;
            }
            Console.WriteLine("Connection successful");

            // Send command to strobe LED
            String[] commands = { "{\"strobe\":null}" };
            connection.SendCommands(commands, 2, 500);

            // Close connection
            System.Threading.Thread.Sleep(60000);
            connection.Close();
        }

        private static String TimestampFormat(UInt64 timestamp)
        {
            return String.Format("{0,8}", timestamp) + " us";
        }

        private static String IntFormat(uint value)
        {
            return " " + String.Format("{0,8}", value);
        }

        private static String IntFormat(ulong value)
        {
            return " " + String.Format("{0,8}", value);
        }

        private static String FloatFormat(float value)
        {
            return " " + String.Format("{0,8:###0.000}", value).Replace(",", ".");
        }

        private static String StringFormat(String value)
        {
            return " \"" + value + "\"";
        }

        private void DecodeErrorEvent(Object sender, Ximu3.DecodeErrorEventArgs args)
        {
            Console.WriteLine(args.decodeError.ToString());
        }

        private void StatisticsEvent(Object sender, Ximu3.StatisticsEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.statistics.Timestamp) +
                IntFormat(args.statistics.DataTotal) + " bytes" +
                IntFormat(args.statistics.DataRate) + " bytes/s" +
                IntFormat(args.statistics.MessageTotal) + " messages" +
                IntFormat(args.statistics.MessageRate) + " messages/s" +
                IntFormat(args.statistics.ErrorTotal) + " errors" +
                IntFormat(args.statistics.ErrorRate) + " errors/s");
            // Console.WriteLine(args.statistics.ToString()); // alternative to above
        }

        private void InertialEvent(Object sender, Ximu3.InertialEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                 FloatFormat(args.message.GyroscopeX) + " deg/s" +
                 FloatFormat(args.message.GyroscopeY) + " deg/s" +
                 FloatFormat(args.message.GyroscopeZ) + " deg/s" +
                 FloatFormat(args.message.AccelerometerX) + " g" +
                 FloatFormat(args.message.AccelerometerY) + " g" +
                 FloatFormat(args.message.AccelerometerZ) + " g");
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void MagnetometerEvent(Object sender, Ximu3.MagnetometerEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                 FloatFormat(args.message.XAxis) + " a.u." +
                 FloatFormat(args.message.YAxis) + " a.u." +
                 FloatFormat(args.message.ZAxis) + " a.u.");
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void QuaternionEvent(Object sender, Ximu3.QuaternionEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.WElement) +
                FloatFormat(args.message.XElement) +
                FloatFormat(args.message.YElement) +
                FloatFormat(args.message.ZElement));
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void RotationMatrixEvent(Object sender, Ximu3.RotationMatrixEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.XXElement) +
                FloatFormat(args.message.XYElement) +
                FloatFormat(args.message.XZElement) +
                FloatFormat(args.message.YXElement) +
                FloatFormat(args.message.YYElement) +
                FloatFormat(args.message.YZElement) +
                FloatFormat(args.message.ZXElement) +
                FloatFormat(args.message.ZYElement) +
                FloatFormat(args.message.ZZElement));
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void EulerAnglesEvent(Object sender, Ximu3.EulerAnglesEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.Roll) + " deg" +
                FloatFormat(args.message.Pitch) + " deg" +
                FloatFormat(args.message.Yaw) + " deg");
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void LinearAccelerationEvent(Object sender, Ximu3.LinearAccelerationEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.WElement) +
                FloatFormat(args.message.XElement) +
                FloatFormat(args.message.YElement) +
                FloatFormat(args.message.ZElement) +
                FloatFormat(args.message.XAxis) + " g" +
                FloatFormat(args.message.YAxis) + " g" +
                FloatFormat(args.message.ZAxis) + " g");
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void EarthAccelerationEvent(Object sender, Ximu3.EarthAccelerationEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.WElement) +
                FloatFormat(args.message.XElement) +
                FloatFormat(args.message.YElement) +
                FloatFormat(args.message.ZElement) +
                FloatFormat(args.message.XAxis) + " g" +
                FloatFormat(args.message.YAxis) + " g" +
                FloatFormat(args.message.ZAxis) + " g");
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void HighGAccelerometerEvent(Object sender, Ximu3.HighGAccelerometerEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.XAxis) + " g" +
                FloatFormat(args.message.YAxis) + " g" +
                FloatFormat(args.message.ZAxis) + " g");
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void TemperatureEvent(Object sender, Ximu3.TemperatureEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.Temperature) + " degC");
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void BatteryEvent(Object sender, Ximu3.BatteryEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.Percentage) + " %" +
                FloatFormat(args.message.Voltage) + " V" +
                FloatFormat(args.message.ChargingStatus));
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void RssiEvent(Object sender, Ximu3.RssiEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) +
                FloatFormat(args.message.Percentage) + " %" +
                FloatFormat(args.message.Power) + " dBm");
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void SerialAccessoryEvent(Object sender, Ximu3.SerialAccessoryEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) + StringFormat(args.message.BytesAsString));
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void NotificationEvent(Object sender, Ximu3.NotificationEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) + StringFormat(args.message.BytesAsString));
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }

        private void ErrorEvent(Object sender, Ximu3.ErrorEventArgs args)
        {
            Console.WriteLine(TimestampFormat(args.message.Timestamp) + StringFormat(args.message.BytesAsString));
            // Console.WriteLine(args.message.ToString()); // alternative to above
        }
    }
}
