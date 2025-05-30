namespace Ximu3Examples
{
    class DataLogger
    {
        public DataLogger()
        {
            // Open all USB connections
            Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUsb);

            List<Ximu3.Connection> connections = [];

            foreach (Ximu3.CApi.XIMU3_Device device in devices)
            {
                Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(device)));
                Ximu3.Connection connection = new(device.usb_connection_info);
                try
                {
                    connection.Open();
                    connections.Add(connection);
                }
                catch
                {
                    Console.WriteLine("Unable to open connection");
                    return;
                }
            }

            if (connections.Count == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            // Log data
            string destination = "C:/";
            string name = "Data Logger Example";

            if (Helpers.YesOrNo("Use async implementation?"))
            {
                using Ximu3.DataLogger dataLogger = new(destination, name, [.. connections]);

                Ximu3.CApi.XIMU3_Result result = dataLogger.GetResult();

                if (result == Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
                {
                    System.Threading.Thread.Sleep(3000);
                }

                PrintResult(result);
            }
            else
            {
                PrintResult(Ximu3.DataLogger.Log(destination, name, [.. connections], 3));
            }

            // Close all connections
            foreach (Ximu3.Connection connection in connections)
            {
                connection.Close();
            }
        }

        private static void PrintResult(Ximu3.CApi.XIMU3_Result result)
        {
            switch (result)
            {
                case Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk:
                    Console.WriteLine("OK");
                    break;
                case Ximu3.CApi.XIMU3_Result.XIMU3_ResultError:
                    Console.WriteLine("Error");
                    break;
            }
        }
    }
}
