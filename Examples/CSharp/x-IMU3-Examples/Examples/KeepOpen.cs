namespace Ximu3Examples
{
    class KeepOpen
    {
        public KeepOpen()
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

            using Ximu3.KeepOpen keepOpen = new(connection, Callback);

            // Close connection
            System.Threading.Thread.Sleep(60000);
            connection.Close();
        }

        private void Callback(Ximu3.CApi.XIMU3_ConnectionStatus status)
        {
            switch (status)
            {
                case Ximu3.CApi.XIMU3_ConnectionStatus.XIMU3_ConnectionStatusConnected:
                    Console.WriteLine("Connected");
                    break;
                case Ximu3.CApi.XIMU3_ConnectionStatus.XIMU3_ConnectionStatusReconnecting:
                    Console.WriteLine("Reconnecting");
                    break;
            }
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_connection_status_to_string(status))); // alternative to above
        }
    }
}
