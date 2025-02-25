namespace Ximu3Examples
{
    class BluetoothConnection : Connection
    {
        public BluetoothConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                Console.WriteLine("Searching for connections");
                Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeBluetooth);
                if (devices.Length == 0)
                {
                    Console.WriteLine("No Bluetooth connections available");
                    return;
                }
                Console.WriteLine("Found " + Ximu3.Helpers.ToString(devices[0].device_name) + " " + Ximu3.Helpers.ToString(devices[0].serial_number));
                Run(devices[0].bluetooth_connection_info);
            }
            else
            {
                Ximu3.CApi.XIMU3_BluetoothConnectionInfo connectionInfo = new()
                {
                    port_name = Ximu3.Helpers.ToBytes("COM1")
                };
                Run(connectionInfo);
            }
        }
    }
}
