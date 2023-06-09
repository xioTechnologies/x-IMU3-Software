using System;

namespace Ximu3Examples
{
    class PortScanner
    {
        public PortScanner()
        {
            if (Helpers.AskQuestion("Use async implementation?"))
            {
                using Ximu3.PortScanner portScanner = new Ximu3.PortScanner(PortScannerEvent);
                System.Threading.Thread.Sleep(60000);
            }
            else
            {
                Ximu3.Device[] devices = Ximu3.PortScanner.Scan();
                Console.WriteLine("Devices updated (" + devices.Length + " devices available)");
                PrintDevices(devices);
            }
        }

        private void PortScannerEvent(Object sender, Ximu3.PortScannerEventArgs args)
        {
            Console.WriteLine("Found " + args.devices.Length + " devices");
            PrintDevices(args.devices);
        }

        private void PrintDevices(Ximu3.Device[] devices)
        {
            foreach (Ximu3.Device device in devices)
            {
                Console.WriteLine(device.DeviceName + ", " + device.SerialNumber + ", " + device.ConnectionInfo);
                // Console.WriteLine(device); // alternative to above
            }
        }
    }
}
