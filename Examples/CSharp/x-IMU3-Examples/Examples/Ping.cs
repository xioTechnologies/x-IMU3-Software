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

            Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(devices[0])));

            // Open connection
            Ximu3.Connection connection = new(Ximu3.ConnectionInfo.From(devices[0])!);

            Ximu3.CApi.XIMU3_Result result = connection.Open();

            if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open " + connection.GetInfo() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
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
                PrintResponse(connection.Ping());
            }

            // Close connection
            connection.Close();
        }

        private void Callback(Ximu3.CApi.XIMU3_PingResponse? response)
        {
            PrintResponse(response);
        }

        static private void PrintResponse(Ximu3.CApi.XIMU3_PingResponse? response)
        {
            if (response == null)
            {
                Console.WriteLine("No response");
                return;
            }

            Console.WriteLine(Ximu3.Helpers.ToString(response.Value.interface_) + ", " + Ximu3.Helpers.ToString(response.Value.device_name) + ", " + Ximu3.Helpers.ToString(response.Value.serial_number));
            // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_ping_response_to_string(response.Value))); // alternative to above
        }
    }
}
