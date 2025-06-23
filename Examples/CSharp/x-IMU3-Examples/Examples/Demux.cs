namespace Ximu3Examples
{
    class Demux
    {
        public Demux()
        {
            // Search for connection
            Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            Console.WriteLine("Found " + Ximu3.Helpers.ToString(devices[0].device_name) + " " + Ximu3.Helpers.ToString(devices[0].serial_number));

            // Open connection
            Ximu3.Connection connection = new(Ximu3.ConnectionInfo.From(devices[0])!);

            if (connection.Open() != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open connection");
                return;
            }

            // Demux
            {
                using Ximu3.Demux demux = new(connection, [0x41, 0x42, 0x43]);

                Ximu3.CApi.XIMU3_Result result = demux.GetResult();

                PrintResult(result);

                if (result == Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
                {
                    System.Threading.Thread.Sleep(60000);
                }
            }

            // Close connection
            connection.Close();
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
