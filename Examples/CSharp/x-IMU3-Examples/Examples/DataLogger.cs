namespace Ximu3Examples
{
    class DataLogger
    {
        public DataLogger()
        {
            // Open all USB connections
            var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            System.Threading.Thread.Sleep(1000); // wait for OS to release port

            List<Ximu3.Connection> connections = [];

            foreach (var device in devices)
            {
                Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(device)));

                var connection = new Ximu3.Connection(Ximu3.ConnectionConfig.From(device)!);

                var result = connection.Open();

                if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
                {
                    Console.WriteLine("Unable to open " + connection.GetConfig() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                }
                else
                {
                    connections.Add(connection);
                }
            }

            if (connections.Count == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            // Log data (blocking)
            var destination = "C:/";
            var nameBlocking = "x-IMU3 Data Logger Example Blocking";

            var resultBlocking = Ximu3.DataLogger.Log(destination, nameBlocking, [.. connections], 3);

            if (resultBlocking != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Data logger failed. " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(resultBlocking)) + ".");
            }

            // Log data (non-blocking)
            var nameNonBlocking = "x-IMU3 Data Logger Example Non-Blocking";

            var dataLogger = new Ximu3.DataLogger(destination, nameNonBlocking, [.. connections]);

            var resultNonBlocking = dataLogger.GetResult();

            if (resultNonBlocking != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Data logger failed. " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(resultNonBlocking)) + ".");
            }

            System.Threading.Thread.Sleep(3000);

            dataLogger.Dispose(); // stop logging

            Console.WriteLine("Complete");

            // Close all connections
            foreach (var connection in connections)
            {
                connection.Close();
            }
        }
    }
}
