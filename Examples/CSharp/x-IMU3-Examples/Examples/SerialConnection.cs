namespace Ximu3Examples
{
    class SerialConnection : Connection
    {
        public SerialConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                Console.WriteLine("Searching for connections");
                Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeSerial);
                if (devices.Length == 0)
                {
                    Console.WriteLine("No serial connections available");
                    return;
                }
                Console.WriteLine("Found " + Ximu3.Helpers.ToString(devices[0].device_name) + " " + Ximu3.Helpers.ToString(devices[0].serial_number));
                Run(devices[0].serial_connection_info);
            }
            else
            {
                Ximu3.CApi.XIMU3_SerialConnectionInfo connectionInfo = new()
                {
                    port_name = Ximu3.Helpers.ToBytes("COM1"),
                    baud_rate = 115200,
                    rts_cts_enabled = false
                };
                Run(connectionInfo);
            }
        }
    }
}
