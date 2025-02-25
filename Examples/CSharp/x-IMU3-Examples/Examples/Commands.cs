namespace Ximu3Examples
{
    class Commands
    {
        public Commands()
        {
            // Search for connection
            Console.WriteLine("Searching for connections");
            Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUsb);
            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }
            Console.WriteLine("Found " + Ximu3.Helpers.ToString(devices[0].device_name) + " " + Ximu3.Helpers.ToString(devices[0].serial_number));

            // Open connection
            Ximu3.Connection connection = new(devices[0].usb_connection_info);
            if (connection.Open() != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open connection");
                return;
            }

            // Example commands
            string[] commands = [
                "{\"device_name\":\"Foobar\"}", // write "Foobar" to device name
                "{\"serial_number\":null}", // read serial number
                "{\"firmware_version\":null}", // read firmware version
                "{\"invalid_key\":null}", // invalid key to demonstrate an error response
            ];

            // Send commands
            if (Helpers.YesOrNo("Use async implementation?"))
            {
                connection.SendCommandsAsync(commands, 2, 500, Callback);
                System.Threading.Thread.Sleep(3000);
            }
            else
            {
                PrintResponses(connection.SendCommands(commands, 2, 500));
            }

            // Close connection
            connection.Close();
        }

        private void Callback(string[] responses)
        {
            PrintResponses(responses);
        }

        private static void PrintResponses(string[] responses)
        {
            Console.WriteLine(responses.Length + " responses received");
            foreach (string response_ in responses)
            {
                Ximu3.CApi.XIMU3_CommandMessage response = Ximu3.CApi.XIMU3_command_message_parse(Ximu3.Helpers.ToPointer(response_));
                if (Ximu3.Helpers.ToString(response.error).Length > 0)
                {
                    Console.WriteLine(Ximu3.Helpers.ToString(response.error));
                    return;
                }
                Console.WriteLine(Ximu3.Helpers.ToString(response.key) + " : " + Ximu3.Helpers.ToString(response.value));
            }
        }
    }
}
