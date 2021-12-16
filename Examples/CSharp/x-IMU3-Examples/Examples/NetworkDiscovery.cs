using System;

namespace Ximu3Examples
{
    class NetworkDiscovery
    {
        public NetworkDiscovery()
        {
            if (Helpers.YesOrNo("Use async implementation?") == true)
            {
                Ximu3.NetworkDiscovery discovery = new Ximu3.NetworkDiscovery(NetworkDiscoveryEvent);
                Helpers.Wait(-1);
                discovery.Dispose();
            }
            else
            {
                Ximu3.DiscoveredNetworkDevice[] devices = Ximu3.NetworkDiscovery.Scan(2000);
                Console.WriteLine("Devices updated (" + devices.Length + " devices available)");
                PrintDevices(devices);
            }
        }

        private void NetworkDiscoveryEvent(Object sender, Ximu3.NetworkDiscoveryEventArgs args)
        {
            Console.WriteLine("Discovered " + args.devices.Length + " devices");
            PrintDevices(args.devices);
        }

        private void PrintDevices(Ximu3.DiscoveredNetworkDevice[] devices)
        {
            foreach (Ximu3.DiscoveredNetworkDevice device in devices)
            {
                Console.WriteLine(device.DeviceName + " - " +
                    device.SerialNumber + ", RSSI: " +
                    device.Rssi + "%, Battery: " +
                    device.Battery + "%, " +
                    device.Status + ", " +
                    device.TcpConnectionInfo + ", " +
                    device.UdpConnectionInfo);
                // Console.WriteLine(device); // alternative to above
            }
        }
    }
}
