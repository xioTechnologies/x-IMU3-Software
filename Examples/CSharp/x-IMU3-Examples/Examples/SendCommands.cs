namespace Ximu3Examples
{
    class SendCommands
    {
        public SendCommands()
        {
            // Search for connection
            var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(devices[0])));

            // Open connection
            var connection = new Ximu3.Connection(Ximu3.ConnectionConfig.From(devices[0])!);

            var result = connection.Open();

            if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open " + connection.GetConfig() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                return;
            }

            // Example commands
            string[] commands =
            [
                "{\"device_name\":\"Foobar\"}", // write "Foobar" to device name
                "{\"serial_number\":null}", // read serial number
                "{\"firmware_version\":null}", // read firmware version
                "{\"invalid_key\":null}", // invalid key to demonstrate an error response
            ];

            // Send commands
            if (Helpers.YesOrNo("Use async implementation?"))
            {
                connection.SendCommandsAsync(commands, Callback);

                System.Threading.Thread.Sleep(3000);
            }
            else
            {
                PrintResponses(connection.SendCommands(commands));
            }

            // Close connection
            connection.Close();
        }

        private void Callback(Ximu3.CommandMessage?[] responses)
        {
            PrintResponses(responses);
        }

        private static void PrintResponses(Ximu3.CommandMessage?[] responses)
        {
            foreach (var response in responses)
            {
                if (response == null)
                {
                    Console.WriteLine("No response");
                    continue;
                }

                if (response.Error != null)
                {
                    Console.WriteLine(response.Error);
                    continue;
                }

                Console.WriteLine(response.Key + " : " + response.Value);
            }
        }
    }
}
