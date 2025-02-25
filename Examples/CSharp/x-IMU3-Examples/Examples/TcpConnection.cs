namespace Ximu3Examples
{
    class TcpConnection : Connection
    {
        public TcpConnection()
        {
            if (Helpers.YesOrNo("Search for connections?"))
            {
                Console.WriteLine("Searching for connections");
                Ximu3.CApi.XIMU3_NetworkAnnouncementMessage[] messages = new Ximu3.NetworkAnnouncement().GetMessagesAfterShortDelay();
                if (messages.Length == 0)
                {
                    Console.WriteLine("No TCP connections available");
                    return;
                }
                Console.WriteLine("Found " + Ximu3.Helpers.ToString(messages[0].device_name) + " " + Ximu3.Helpers.ToString(messages[0].serial_number));
                Run(Ximu3.CApi.XIMU3_network_announcement_message_to_tcp_connection_info(messages[0]));
            }
            else
            {
                Ximu3.CApi.XIMU3_TcpConnectionInfo connectionInfo = new();
                connectionInfo.ip_address = Ximu3.Helpers.ToBytes("192.168.1.1");
                connectionInfo.port = 7000;
                Run(connectionInfo);
            }
        }
    }
}
