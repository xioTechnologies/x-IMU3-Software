using System;

namespace Ximu3Examples
{
    class SerialDiscovery
    {
        public SerialDiscovery()
        {
            if (Helpers.YesOrNo("Use async implementation?") == true)
            {
                Ximu3.SerialDiscovery discovery = new Ximu3.SerialDiscovery(SerialDiscoveryEvent);
                Helpers.Wait(-1);
                discovery.Dispose();
            }
            else
            {
                Ximu3.DiscoveredSerialDevice[] devices = Ximu3.SerialDiscovery.Scan(2000);
                Console.WriteLine("Devices updated (" + devices.Length + " devices available)");
                PrintDevices(devices);
            }
        }

        private void SerialDiscoveryEvent(Object sender, Ximu3.SerialDiscoveryEventArgs args)
        {
            Console.WriteLine("Discovered " + args.devices.Length + " devices");
            PrintDevices(args.devices);
        }

        private void PrintDevices(Ximu3.DiscoveredSerialDevice[] devices)
        {
            foreach (Ximu3.DiscoveredSerialDevice device in devices)
            {
                Console.WriteLine(device.DeviceName + " - " + device.SerialNumber + ", " + device.ConnectionInfo);
                // Console.WriteLine(device); // alternative to above
            }
        }
    }
}
