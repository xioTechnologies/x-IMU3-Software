namespace Ximu3Examples
{
    class MuxConnection : Connection
    {
        public MuxConnection()
        {
            // Search for connection
            Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            Console.WriteLine("Found " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_device_to_string(devices[0])));

            // Open connection
            Ximu3.Connection usbConnection = new(Ximu3.ConnectionInfo.From(devices[0])!);

            if (usbConnection.Open() != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open connection");
                return;
            }

            Ximu3.MuxConnectionInfo connectionInfo = new(0x41, usbConnection);

            Run(connectionInfo);
        }
    }
}
