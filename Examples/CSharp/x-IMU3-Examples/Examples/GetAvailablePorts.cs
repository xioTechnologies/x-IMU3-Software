using System;

namespace Ximu3Examples
{
    class GetAvailablePorts
    {
        public GetAvailablePorts()
        {
            string[] portNames = Ximu3.SerialDiscovery.GetAvailablePorts();

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
