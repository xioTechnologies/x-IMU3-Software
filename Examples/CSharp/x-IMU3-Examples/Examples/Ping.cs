using System;

namespace Ximu3Examples
{
    class Ping
    {
        public Ping()
        {
            // Create connection info
            Ximu3.UsbConnectionInfo connectionInfo = new Ximu3.UsbConnectionInfo("COM1");

            // Open and ping
            Ximu3.Connection connection = new Ximu3.Connection(connectionInfo);
            if (Helpers.YesOrNo("Use async implementation?"))
            {
                connection.OpenAsync(OpenEvent);
                System.Threading.Thread.Sleep(3000);
            }
            else
            {
                if (connection.Open() != Ximu3.Result.Ok)
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
            if (pingResponse.Result != Ximu3.Result.Ok)
            {
                Console.WriteLine("No response");
                return;
            }
            Console.WriteLine(pingResponse.Interface + ", " + pingResponse.DeviceName + ", " + pingResponse.SerialNumber);
            // Console.WriteLine(pingResponse); // alternative to above
        }
    }
}
