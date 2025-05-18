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
                string connectionInfo = "";

                switch (device.connection_type)
                {
                    case Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUsb:
                        connectionInfo = Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_usb_connection_info_to_string(device.usb_connection_info));
                        break;
                    case Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeSerial:
                        connectionInfo = Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_serial_connection_info_to_string(device.serial_connection_info));
                        break;
                    case Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeBluetooth:
                        connectionInfo = Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_bluetooth_connection_info_to_string(device.bluetooth_connection_info));
                        break;
                }

                Console.WriteLine(Ximu3.Helpers.ToString(device.device_name) + ", " + Ximu3.Helpers.ToString(device.serial_number) + ", " + connectionInfo);
                //Console.WriteLine(Ximu3.Helpers.ToString(CApi.XIMU3_device_to_string(device))); // alternative to above
            }
        }
    }
}
