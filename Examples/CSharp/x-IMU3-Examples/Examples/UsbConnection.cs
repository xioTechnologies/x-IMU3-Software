namespace Ximu3Examples
{
    class UsbConnection : Connection
    {
        public UsbConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

                if (devices.Length == 0)
                {
                    Console.WriteLine("No USB connections available");
                    return;
                }

                Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(devices[0])));

                var config = Ximu3.ConnectionConfig.From(devices[0])!;

                Run(config);
            }
            else
            {
                var config = new Ximu3.UsbConnectionConfig("COM1"); // replace with actual connection config

                Run(config);
            }
        }
    }
}
