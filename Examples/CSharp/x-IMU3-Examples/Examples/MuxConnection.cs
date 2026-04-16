namespace Ximu3Examples
{
    class MuxConnection : Connection
    {
        public MuxConnection()
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

            // Mux connection
            if (Helpers.YesOrNo("Connect automatically?"))
            {
                var muxDevices = Ximu3.MuxScanner.Scan(usbConnection);

                if (muxDevices.Length == 0)
                {
                    Console.WriteLine("No mux connections available");
                    usbConnection.Close();
                    return;
                }

                var config = Ximu3.ConnectionConfig.From(muxDevices[0]);

                Run(config!);
            }
            else
            {
                var config = new Ximu3.MuxConnectionConfig(0x41, usbConnection); // replace with actual connection config

                Run(config);
            }

            // Close connection
            usbConnection.Close();
        }
    }
}
