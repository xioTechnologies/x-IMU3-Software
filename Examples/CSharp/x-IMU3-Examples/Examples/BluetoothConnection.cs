namespace Ximu3Examples
{
    class BluetoothConnection : Connection
    {
        public BluetoothConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                var devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeBluetooth);

                if (devices.Length == 0)
                {
                    Console.WriteLine("No Bluetooth connections available");
                    return;
                }

                Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(devices[0])));

                var config = Ximu3.ConnectionConfig.From(devices[0])!;

                Run(config);
            }
            else
            {
                var config = new Ximu3.BluetoothConnectionConfig("COM1"); // replace with actual connection config

                Run(config);
            }
        }
    }
}
