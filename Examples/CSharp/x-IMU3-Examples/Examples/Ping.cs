namespace Ximu3Examples
{
    class Ping
    {
        public Ping()
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

            // Ping
            if (Helpers.YesOrNo("Use async implementation?"))
            {
                connection.PingAsync(Callback);

                System.Threading.Thread.Sleep(3000);
            }
            else
            {
                PrintPingResponse(connection.Ping());
            }

            // Close connection
            connection.Close();
        }

        private void Callback(Ximu3.CApi.XIMU3_PingResponse response)
        {
            PrintPingResponse(response);
        }

        static private void PrintPingResponse(Ximu3.CApi.XIMU3_PingResponse response)
        {
            if (response.result == Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine(Ximu3.Helpers.ToString(response.interface_) + ", " + Ximu3.Helpers.ToString(response.device_name) + ", " + Ximu3.Helpers.ToString(response.serial_number));
                // Console.WriteLine(Ximu3.Helpers.ToString(CApi.XIMU3_ping_response_to_string(response))); // alternative to above
            }
            else
            {
                Console.WriteLine("No response");
            }
        }
    }
}
