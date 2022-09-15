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
            if (Helpers.YesOrNo("Use async implementation?") == true)
            {
                Ximu3.DataLogger dataLogger = new Ximu3.DataLogger(directory, name, connections.ToArray(), DataLoggerEvent);
                Helpers.Wait(3);
                dataLogger.Dispose();
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

        private void DataLoggerEvent(Object sender, Ximu3.DataLoggerEventArgs args)
        {
            PrintResult(args.result);
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
