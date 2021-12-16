using System;

namespace Ximu3Examples
{
    class UdpConnection : Connection
    {
        public UdpConnection()
        {
            if (Helpers.YesOrNo("Search for connections?") == true)
            {
                Console.WriteLine("Searching for connections");
                Ximu3.DiscoveredNetworkDevice[] devices = Ximu3.NetworkDiscovery.Scan(2000);
                if (devices.Length == 0)
                {
                    Console.WriteLine("No UDP connections available");
                    return;
                }
                Console.WriteLine("Found " + devices[0].DeviceName + " - " + devices[0].SerialNumber);
                Run(devices[0].UdpConnectionInfo);
            }
            else
            {
                Run(new Ximu3.UdpConnectionInfo("192.168.1.1", 9000, 8000));
            }
        }
    }
}
