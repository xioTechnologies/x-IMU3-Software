namespace Ximu3Examples
{
    class SerialConnection : Connection
    {
        public SerialConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeSerial);

                System.Threading.Thread.Sleep(1000); // wait for OS to release port

                if (devices.Length == 0)
                {
                    Console.WriteLine("No serial connections available");
                    return;
                }

                Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(devices[0])));

                var config = Ximu3.ConnectionConfig.From(devices[0])!;

                Run(config);
            }
            else
            {
                var config = new Ximu3.SerialConnectionConfig("COM1", 115200, false); // replace with actual connection config

                Run(config);
            }
        }
    }
}
