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

            // Define read/write setting commands
            string[] commands = {
                "{\"device_name\":null}", /* change null to a value to write setting */
                "{\"serial_number\":null}",
                "{\"firmware_version\":null}",
                "{\"bootloader_version\":null}",
                "{\"hardware_version\":null}",
                "{\"invalid_setting_key\":null}", /* invalid key to demonstrate an error response */
            };

            // Send commands
            if (Helpers.AskQuestion("Use async implementation?"))
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
            Console.WriteLine(responses.Length + " commands confirmed");
            foreach (string response in responses)
            {
                Console.WriteLine(response);
            }
        }
    }
}
