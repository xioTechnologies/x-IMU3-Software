namespace Ximu3Examples
{
    class PortScanner
    {
        public PortScanner()
        {
            if (Helpers.YesOrNo("Use async implementation?"))
            {
                using Ximu3.PortScanner portScanner = new(Callback);

                System.Threading.Thread.Sleep(60000);
            }
            else
            {
                Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.Scan();

                Console.WriteLine("Devices updated (" + devices.Length + " devices available)");

                PrintDevices(devices);
            }
        }

        private void Callback(Ximu3.CApi.XIMU3_Device[] devices)
        {
            Console.WriteLine("Found " + devices.Length + " devices");

            PrintDevices(devices);
        }

        private static void PrintDevices(Ximu3.CApi.XIMU3_Device[] devices)
        {
            foreach (Ximu3.CApi.XIMU3_Device device in devices)
            {
                Console.WriteLine(Ximu3.Helpers.ToString(device.device_name) + ", " + Ximu3.Helpers.ToString(device.serial_number) + ", " + Ximu3.ConnectionConfig.From(device));
                //Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(device))); // alternative to above
            }
        }
    }
}
