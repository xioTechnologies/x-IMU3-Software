namespace Ximu3Examples
{
    class BluetoothConnection : Connection
    {
        public BluetoothConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeBluetooth);

                if (devices.Length == 0)
                {
                    Console.WriteLine("No Bluetooth connections available");
                    return;
                }

                Console.WriteLine("Found " + Ximu3.Helpers.ToString(devices[0].device_name) + " " + Ximu3.Helpers.ToString(devices[0].serial_number));

                Ximu3.CApi.XIMU3_BluetoothConnectionInfo connectionInfo = devices[0].bluetooth_connection_info;

                Run(connectionInfo);
            }
            else
            {
                Ximu3.CApi.XIMU3_BluetoothConnectionInfo connectionInfo = new()
                {
                    port_name = Ximu3.Helpers.ToBytes("COM1"),
                }; // replace with actual connection info

                Run(connectionInfo);
            }
        }
    }
}
