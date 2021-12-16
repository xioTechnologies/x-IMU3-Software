using System;

namespace Ximu3Examples
{
    class TcpConnection : Connection
    {
        public TcpConnection()
        {
            if (Helpers.YesOrNo("Search for connections?") == true)
            {
                Console.WriteLine("Searching for connections");
                Ximu3.DiscoveredNetworkDevice[] devices = Ximu3.NetworkDiscovery.Scan(2000);
                if (devices.Length == 0)
                {
                    Console.WriteLine("No TCP connections available");
                    return;
                }
                Console.WriteLine("Found " + devices[0].DeviceName + " - " + devices[0].SerialNumber);
                Run(devices[0].TcpConnectionInfo);
            }
            else
            {
                Run(new Ximu3.TcpConnectionInfo("192.168.1.1", 7000));
            }
        }
    }
}
