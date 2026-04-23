namespace Ximu3Examples
{
    class UsbConnection : Connection
    {
        public UsbConnection()
        {
            if (Helpers.YesOrNo("Connect automatically?"))
            {
                var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

                System.Threading.Thread.Sleep(1000); // wait for OS to release port

                if (devices.Length == 0)
                {
                    Console.WriteLine("No USB connections available");
                    return;
                }

                Console.WriteLine("Found " + devices[0].ToString());

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
