using System;

namespace Ximu3Examples
{
    class BluetoothConnection : Connection
    {
        public BluetoothConnection()
        {
            if (Helpers.YesOrNo("Search for connections?") == true)
            {
                Console.WriteLine("Searching for connections");
                Ximu3.Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.ConnectionType.Bluetooth);
                if (devices.Length == 0)
                {
                    Console.WriteLine("No Bluetooth connections available");
                    return;
                }
                Console.WriteLine("Found " + devices[0].DeviceName + " - " + devices[0].SerialNumber);
                Run(devices[0].ConnectionInfo);
            }
            else
            {
                Run(new Ximu3.BluetoothConnectionInfo("COM1"));
            }
        }
    }
}
