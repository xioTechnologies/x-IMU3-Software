using System;

namespace Ximu3Examples
{
    class Commands
    {
        public Commands()
        {
            // Search for connection
            Console.WriteLine("Searching for connections");
            Ximu3.Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.ConnectionType.Usb);
            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }
            Console.WriteLine("Found " + devices[0].DeviceName + " " + devices[0].SerialNumber);

            // Open connection
            Ximu3.Connection connection = new Ximu3.Connection(devices[0].ConnectionInfo);
            if (connection.Open() != Ximu3.Result.Ok)
            {
                Console.WriteLine("Unable to open connection");
                return;
            }

            // Example commands
            string[] commands = {
                "{\"device_name\":\"Foobar\"}", // write "Foobar" to device name
                "{\"serial_number\":null}", // read serial number
                "{\"firmware_version\":null}", // read firmware version
                "{\"invalid_key\":null}", // invalid key to demonstrate an error response
            };

            // Send commands
            if (Helpers.YesOrNo("Use async implementation?"))
            {
                connection.SendCommandsAsync(commands, 2, 500, SendCommandsEvent);
                System.Threading.Thread.Sleep(3000);
            }
            else
            {
                PrintResponses(connection.SendCommands(commands, 2, 500));
            }

            // Close connection
            connection.Close();
        }

        private void SendCommandsEvent(Object sender, Ximu3.SendCommandsEventArgs args)
        {
            PrintResponses(args.responses);
        }

        private void PrintResponses(string[] responses)
        {
            Console.WriteLine(responses.Length + " responses received");
            foreach (string response_ in responses)
            {
                Ximu3.CommandMessage response = Ximu3.CommandMessage.Parse(response_);
                if (response.Error.Length > 0)
                {
                    Console.WriteLine(response.Error);
                    return;
                }
                Console.WriteLine(response.Key + " : " + response.Value);
            }
        }
    }
}
