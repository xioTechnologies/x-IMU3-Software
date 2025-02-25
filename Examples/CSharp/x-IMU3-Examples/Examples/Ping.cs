namespace Ximu3Examples
{
    class Ping
    {
        public Ping()
        {
            // Create connection info
            Ximu3.CApi.XIMU3_UsbConnectionInfo connectionInfo = new()
            {
                port_name = Ximu3.Helpers.ToBytes("COM1")
            };

            // Open and ping
            connection = new Ximu3.Connection(connectionInfo);
            if (Helpers.YesOrNo("Use async implementation?"))
            {
                connection.OpenAsync(Callback);
                System.Threading.Thread.Sleep(3000);
            }
            else
            {
                if (connection.Open() != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
                {
                    Console.WriteLine("Unable to open connection");
                    return;
                }
                PrintPingResponse(connection.Ping());
            }

            // Close connection
            connection.Close();
        }

        private void Callback(Ximu3.CApi.XIMU3_Result result)
        {
            if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open connection");
                return;
            }
            PrintPingResponse(connection.Ping());
        }

        private static void PrintPingResponse(Ximu3.CApi.XIMU3_PingResponse pingResponse)
        {
            if (pingResponse.result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("No response");
                return;
            }
            Console.WriteLine(Ximu3.Helpers.ToString(pingResponse.interface_) + ", " + Ximu3.Helpers.ToString(pingResponse.device_name) + ", " + Ximu3.Helpers.ToString(pingResponse.serial_number));
            // Console.WriteLine(Ximu3.Helpers.ToString(CApi.XIMU3_ping_response_to_string(pingResponse))); // alternative to above
        }

        private readonly Ximu3.Connection connection;
    }
}
