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
            Console.WriteLine("Found " + devices[0].DeviceName + " - " + devices[0].SerialNumber);

            // Open connection
            Ximu3.Connection connection = new Ximu3.Connection(devices[0].ConnectionInfo);
            if (connection.Open() != Ximu3.Result.Ok)
            {
                Console.WriteLine("Unable to open connection");
                return;
            }
            Console.WriteLine("Connection successful");

            // Define read/write setting commands
            String[] commands = {
                "{\"deviceName\":null}", /* change null to a value to write setting */
                "{\"serialNumber\":null}",
                "{\"firmwareVersion\":null}",
                "{\"bootloaderVersion\":null}",
                "{\"hardwareVersion\":null}",
                "{\"invalidSettingKey\":null}", /* this command is deliberately invalid to demonstrate a failed command */
            };

            // Send commands
            if (Helpers.YesOrNo("Use async implementation?") == true)
            {
                connection.SendCommandsAsync(commands, 2, 500, SendCommandsEvent);
                Helpers.Wait(3);
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

        private void PrintResponses(String[] responses)
        {
            Console.WriteLine(responses.Length + " commands confirmed");
            foreach (String response in responses)
            {
                Console.WriteLine(response);
            }
        }
    }
}
