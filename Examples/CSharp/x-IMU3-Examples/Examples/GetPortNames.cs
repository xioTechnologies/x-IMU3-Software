namespace Ximu3Examples
{
    class GetPortNames
    {
        public GetPortNames()
        {
            var portNames = Ximu3.PortScanner.GetPortNames();

            if (portNames.Length == 0)
            {
                Console.WriteLine("No ports available");
                return;
            }

            foreach (var portName in portNames)
            {
                Console.WriteLine(portName);
            }
        }
    }
}
