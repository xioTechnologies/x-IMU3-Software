namespace Ximu3Examples
{
    class NetworkAnnouncement
    {
        public NetworkAnnouncement()
        {
            Ximu3.NetworkAnnouncement networkAnnouncement = new();

            if (networkAnnouncement.GetResult() != Ximu3.CApi.XIMU3_Result.XIMU3_ResultOk)
            {
                Console.WriteLine("Unable to open network announcement socket");
                return;
            }

            if (Helpers.YesOrNo("Use async implementation?"))
            {
                networkAnnouncement.AddCallback(Callback);

                System.Threading.Thread.Sleep(60000);
            }
            else
            {
                foreach (Ximu3.CApi.XIMU3_NetworkAnnouncementMessage message in networkAnnouncement.GetMessages())
                {
                    PrintMessage(message);
                }
            }
        }

        private void Callback(Ximu3.CApi.XIMU3_NetworkAnnouncementMessage message)
        {
            PrintMessage(message);
        }

        private static void PrintMessage(Ximu3.CApi.XIMU3_NetworkAnnouncementMessage message)
        {
            Console.WriteLine(Ximu3.Helpers.ToString(message.device_name) + ", " +
                Ximu3.Helpers.ToString(message.serial_number) + ", " +
                Ximu3.Helpers.ToString(message.ip_address) + ", " +
                message.tcp_port + ", " +
                message.udp_send + ", " +
                message.udp_receive + ", " +
                message.rssi + "%, " +
                message.battery + "%, " +
                Ximu3.Helpers.ToString(Ximu3.CApi.XIMU3_charging_status_to_string(message.charging_status)));
            // Console.WriteLine(Ximu3.Helpers.ToString(CApi.XIMU3_network_announcement_message_to_string(message))); // alternative to above
        }
    }
}
