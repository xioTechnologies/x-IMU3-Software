namespace Ximu3Examples
{
    class SerialAccessory
    {
        public SerialAccessory()
        {
            // Search for connection
            var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            System.Threading.Thread.Sleep(1000); // wait for OS to release port

            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            Console.WriteLine("Found " + devices[0].ToString());

            // Open connection
            var connection = new Ximu3.Connection(Ximu3.ConnectionConfig.From(devices[0])!);

            var result = connection.Open();

            if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open " + connection.GetConfig() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                return;
            }

            // Send accessory data
            byte[] data = "Any value \x00 to \xFF\n"u8.ToArray();

            var dataAsJson = Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_bytes_to_json(data, (uint)data.Length));

            var response = connection.SendCommand("{\"accessory\":" + dataAsJson + "}");

            if (response == null)
            {
                Console.WriteLine("No response");
            }
            else if (response.Error != null)
            {
                Console.WriteLine(response.Error);
            }
            else
            {
                PrintData("TX", data);
            }

            // Receive accessory data
            System.Threading.Thread.Sleep(1000);

            if (connection.GetSerialAccessoryMessage() is { } message)
            {
                PrintData("RX", message.char_array.Take((int)message.number_of_bytes).ToArray());
            }

            // Close connection
            connection.Close();
        }

        private static void PrintData(string direction, byte[] data)
        {
            Console.WriteLine(direction + " " + string.Join(" ", data.Select(d => d.ToString("X2"))));
        }
    }
}
