namespace Ximu3Examples
{
    class MuxConnection : Connection
    {
        public MuxConnection()
        {
            // Search for connection
            var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            System.Threading.Thread.Sleep(1000); // wait for OS to release port

            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(devices[0])));

            // Open connection
            var usbConnection = new Ximu3.Connection(Ximu3.ConnectionConfig.From(devices[0])!);

            var result = usbConnection.Open();

            if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open " + usbConnection.GetConfig() + ". " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                return;
            }

            var config = new Ximu3.MuxConnectionConfig(0x41, usbConnection);

            Run(config);
        }
    }
}
