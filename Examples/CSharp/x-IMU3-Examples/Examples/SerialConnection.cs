namespace Ximu3Examples
{
    class SerialConnection : Connection
    {
        public SerialConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeSerial);

                if (devices.Length == 0)
                {
                    Console.WriteLine("No serial connections available");
                    return;
                }

                Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(devices[0])));

                Ximu3.ConnectionInfo connectionInfo = Ximu3.ConnectionInfo.From(devices[0])!;

                Run(connectionInfo);
            }
            else
            {
                Ximu3.SerialConnectionInfo connectionInfo = new("COM1", 115200, false); // replace with actual connection info

                Run(connectionInfo);
            }
        }
    }
}
