namespace Ximu3Examples
{
    class OpenAsync
    {
        public OpenAsync()
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
            Ximu3.Connection connection = new(Ximu3.ConnectionConfig.From(devices[0])!);

            Ximu3.Connection.OpenAsyncCallback callback = result =>
            {
                if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
                {
                    Console.WriteLine("Unable to open " + connection.GetConfig() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                    return;
                }

                Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)));
            };

            connection.OpenAsync(callback);

            // Close connection
            System.Threading.Thread.Sleep(3000);

            connection.Close();
        }
    }
}
