﻿namespace Ximu3Examples
{
    class OpenAsync
    {
        public OpenAsync()
        {
            // Search for connection
            Ximu3.CApi.XIMU3_Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.CApi.XIMU3_PortType.XIMU3_PortTypeUsb);

            if (devices.Length == 0)
            {
                Console.WriteLine("No USB connections available");
                return;
            }

            Console.WriteLine("Found " + Ximu3.Helpers.ToString(devices[0].device_name) + " " + Ximu3.Helpers.ToString(devices[0].serial_number));

            // Open connection
            Ximu3.Connection connection = new(Ximu3.ConnectionInfo.From(devices[0])!);

            connection.OpenAsync(Callback);

            // Close connection
            System.Threading.Thread.Sleep(3000);
            connection.Close();
        }

        private void Callback(Ximu3.CApi.XIMU3_Result result)
        {
            Console.WriteLine(Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)));
        }
    }
}
