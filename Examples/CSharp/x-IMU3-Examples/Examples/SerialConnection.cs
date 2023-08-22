using System;

namespace Ximu3Examples
{
    class SerialConnection : Connection
    {
        public SerialConnection()
        {
            if (Helpers.AskQuestion("Search for connections?"))
            {
                Console.WriteLine("Searching for connections");
                Ximu3.Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.ConnectionType.Serial);
                if (devices.Length == 0)
                {
                    Console.WriteLine("No serial connections available");
                    return;
                }
                Console.WriteLine("Found " + devices[0].DeviceName + " " + devices[0].SerialNumber);
                Run(devices[0].ConnectionInfo);
            }
            else
            {
                Run(new Ximu3.SerialConnectionInfo("COM1", 115200, false));
            }
        }
    }
}
