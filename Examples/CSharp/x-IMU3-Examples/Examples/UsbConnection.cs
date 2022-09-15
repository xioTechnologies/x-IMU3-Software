using System;

namespace Ximu3Examples
{
    class UsbConnection : Connection
    {
        public UsbConnection()
        {
            if (Helpers.YesOrNo("Search for connections?") == true)
            {
                Console.WriteLine("Searching for connections");
                Ximu3.Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.ConnectionType.Usb);
                if (devices.Length == 0)
                {
                    Console.WriteLine("No USB connections available");
                    return;
                }
                Console.WriteLine("Found " + devices[0].DeviceName + " - " + devices[0].SerialNumber);
                Run(devices[0].ConnectionInfo);
            }
            else
            {
                Run(new Ximu3.UsbConnectionInfo("COM1"));
            }
        }
    }
}
