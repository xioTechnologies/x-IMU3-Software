namespace Ximu3Examples
{
    class MuxScanner
    {
        public MuxScanner()
        {
            // Search for connection
            var usbDevices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            System.Threading.Thread.Sleep(1000); // wait for OS to release port

            if (usbDevices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(usbDevices[0])));

            // Open connection
            var usbConnection = new Ximu3.Connection(Ximu3.ConnectionConfig.From(usbDevices[0])!);

            var result = usbConnection.Open();

            if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open " + usbConnection.GetConfig() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                return;
            }

            // Mux scanner (blocking)
            var muxDevices = Ximu3.MuxScanner.Scan(usbConnection);

            PrintDevices(muxDevices);

            // Mux scanner (non-blocking)
            var muxScanner = new Ximu3.MuxScanner(usbConnection, Callback);

            System.Threading.Thread.Sleep(60000);

            // Close connection
            usbConnection.Close();
        }

        private void Callback(Ximu3.CApi.XIMU3_Device[] devices)
        {
            PrintDevices(devices);
        }

        private static void PrintDevices(Ximu3.CApi.XIMU3_Device[] devices)
        {
            Console.WriteLine(devices.Length + " device(s) found");

            foreach (var device in devices)
            {
                Console.WriteLine(
                    Ximu3.Helpers.ToString(device.device_name) + ", " +
                    Ximu3.Helpers.ToString(device.serial_number) + ", " +
                    Ximu3.ConnectionConfig.From(device)
                );
                // Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(device))); // alternative to above
            }
        }
    }
}
