namespace Ximu3Examples
{
    class UdpConnection : Connection
    {
        public UdpConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                Ximu3.NetworkAnnouncement networkAnnouncement = new();

                Ximu3.CApi.XIMU3_Result result = networkAnnouncement.GetResult();

                if (result != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
                {
                    Console.WriteLine("Network announcement failed. " + Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_result_to_string(result)) + ".");
                    return;
                }

                Ximu3.CApi.XIMU3_NetworkAnnouncementMessage[] messages = networkAnnouncement.GetMessagesAfterShortDelay();

                if (messages.Length == 0)
                {
                    Console.WriteLine("No UDP connections available");
                    return;
                }

                Console.WriteLine("Found " + Ximu3.Helpers.ToString(messages[0].device_name) + " " + Ximu3.Helpers.ToString(messages[0].serial_number));

                Ximu3.UdpConnectionInfo connectionInfo = new(messages[0]);

                Run(connectionInfo);
            }
            else
            {
                Ximu3.UdpConnectionInfo connectionInfo = new("192.168.1.1", 9000, 8000); // replace with actual connection info

                Run(connectionInfo);
            }
        }
    }
}
