using System;

namespace Ximu3Examples
{
    class GetPortNames
    {
        public GetPortNames()
        {
            string[] portNames = Ximu3.PortScanner.GetPortNames();

            if (portNames.Length == 0)
            {
                Console.WriteLine("No ports available");
                return;
            }

            foreach (string portName in portNames)
            {
                Console.WriteLine(portName);
            }
        }
    }
}
