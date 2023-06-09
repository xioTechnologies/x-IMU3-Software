using System;
using System.Collections.Generic;

namespace Ximu3Examples
{
    class DataLogger
    {
        public DataLogger()
        {
            // Open all USB connections
            Ximu3.Device[] devices = Ximu3.PortScanner.ScanFilter(Ximu3.ConnectionType.Usb);
            List<Ximu3.Connection> connections = new List<Ximu3.Connection>();
            foreach (Ximu3.Device device in devices)
            {
                Console.WriteLine(device.ToString());
                Ximu3.Connection connection = new Ximu3.Connection(device.ConnectionInfo);
                try
                {
                    connection.Open();
                    connections.Add(connection);
                }
                catch
                {
                    Console.WriteLine("Unable to open connection");
                    return;
                }
            }

            // Log data
            string directory = "C:/";
            string name = "Data Logger Example";
            if (Helpers.AskQuestion("Use async implementation?"))
            {
                using Ximu3.DataLogger dataLogger = new Ximu3.DataLogger(directory, name, connections.ToArray());

                Ximu3.Result result = dataLogger.GetResult();

                if (result == Ximu3.Result.Ok)
                {
                    System.Threading.Thread.Sleep(3000);
                }

                PrintResult(result);
            }
            else
            {
                PrintResult(Ximu3.DataLogger.Log(directory, name, connections.ToArray(), 3));
            }

            // Close all connections
            foreach (Ximu3.Connection connection in connections)
            {
                connection.Close();
            }
        }

        private void PrintResult(Ximu3.Result result)
        {
            switch (result)
            {
                case Ximu3.Result.Ok:
                    Console.WriteLine("Ok");
                    break;
                case Ximu3.Result.Error:
                    Console.WriteLine("Error");
                    break;
            }
        }
    }
}
