namespace Ximu3Examples
{
    class UsbConnection : Connection
    {
        public UsbConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_ConnectionType.XIMU3_ConnectionTypeUsb);

                if (devices.Length == 0)
                {
                    Console.WriteLine("No USB connections available");
                    return;
                }

                Console.WriteLine("Found " + Ximu3.Helpers.ToString(devices[0].device_name) + " " + Ximu3.Helpers.ToString(devices[0].serial_number));

                Ximu3.CApi.XIMU3_UsbConnectionInfo connectionInfo = devices[0].usb_connection_info;

                Run(connectionInfo);
            }
            else
            {
                Ximu3.CApi.XIMU3_UsbConnectionInfo connectionInfo = new()
                {
                    port_name = Ximu3.Helpers.ToBytes("COM1"),
                }; // replace with actual connection info

                Run(connectionInfo);
            }
        }
    }
}
