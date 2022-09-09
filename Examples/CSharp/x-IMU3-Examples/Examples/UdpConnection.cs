using System;

namespace Ximu3Examples
{
    class UdpConnection : Connection
    {
        public UdpConnection()
        {
            if (Helpers.YesOrNo("Search for connections?") == true)
            {
                Console.WriteLine("Searching for connections");
                Ximu3.NetworkAnnouncementMessage[] messages = new Ximu3.NetworkAnnouncement().GetMessagesAfterShortDelay();
                if (messages.Length == 0)
                {
                    Console.WriteLine("No UDP connections available");
                    return;
                }
                Console.WriteLine("Found " + messages[0].DeviceName + " - " + messages[0].SerialNumber);
                Run(messages[0].UdpConnectionInfo);
            }
            else
            {
                Run(new Ximu3.UdpConnectionInfo("192.168.1.1", 9000, 8000));
            }
        }
    }
}
