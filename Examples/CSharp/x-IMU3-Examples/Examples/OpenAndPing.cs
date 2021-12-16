using System;

namespace Ximu3Examples
{
    class OpenAndPing
    {
        public OpenAndPing()
        {
            // Create connection info
            Ximu3.UsbConnectionInfo connectionInfo = new Ximu3.UsbConnectionInfo("COM1");

            // Open and ping
            Ximu3.Connection connection = new Ximu3.Connection(connectionInfo);
            if (Helpers.YesOrNo("Use async implementation?"))
            {
                connection.OpenAsync(OpenEvent);
                Helpers.Wait(3);
            }
            else
            {
                try
                {
                    connection.Open();
                }
                catch
                {
                    Console.WriteLine("Unable to open connection");
                    return;
                }
                PrintPingResponse(connection.Ping());
            }

            // Close connection
            connection.Close();
        }

        private void OpenEvent(Object sender, Ximu3.OpenEventArgs args)
        {
            if (args.result != Ximu3.Result.Ok)
            {
                Console.WriteLine("Unable to open connection");
                return;
            }
            PrintPingResponse(((Ximu3.Connection)sender).Ping());
        }

        private void PrintPingResponse(Ximu3.PingResponse pingResponse)
        {
            if (pingResponse.Interface.Length == 0)
            {
                Console.WriteLine("Ping failed");
                return;
            }
            Console.WriteLine(pingResponse.Interface + ", " + pingResponse.DeviceName + " - " + pingResponse.SerialNumber);
            // Console.WriteLine(pingResponse); // alternative to above
        }
    }
}
