namespace Ximu3Examples
{
    class PortScanner
    {
        public PortScanner()
        {
            // Blocking
            var devices = Ximu3.PortScanner.Scan();

            PrintDevices(devices);

            // Non-blocking
            using var portScanner = new Ximu3.PortScanner(Callback);

            System.Threading.Thread.Sleep(60000);
        }

        private void Callback(Ximu3.Device[] devices)
        {
            PrintDevices(devices);
        }

        private static void PrintDevices(Ximu3.Device[] devices)
        {
            Console.WriteLine(devices.Length + " device(s) found");

            foreach (var device in devices)
            {
                Console.WriteLine(
                    Ximu3.Helpers.ToString(device.wrapped.device_name) + ", " +
                    Ximu3.Helpers.ToString(device.wrapped.serial_number) + ", " +
                    Ximu3.ConnectionConfig.From(device)
                );
                // Console.WriteLine(device.ToString()); // alternative to above
            }
        }
    }
}
