namespace Ximu3Examples
{
    class SerialConnection : Connection
    {
        public SerialConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeSerial);

                if (devices.Length == 0)
                {
                    Console.WriteLine("No serial connections available");
                    return;
                }

                Console.WriteLine("Found " + Ximu3.Helpers.ToString(devices[0].device_name) + " " + Ximu3.Helpers.ToString(devices[0].serial_number));

                Ximu3.CApi.XIMU3_SerialConnectionInfo connectionInfo = devices[0].serial_connection_info;

                Run(connectionInfo);
            }
            else
            {
                Ximu3.CApi.XIMU3_SerialConnectionInfo connectionInfo = new()
                {
                    port_name = Ximu3.Helpers.ToBytes("COM1"),
                    baud_rate = 115200,
                    rts_cts_enabled = false,
                }; // replace with actual connection info

                Run(connectionInfo);
            }
        }
    }
}
