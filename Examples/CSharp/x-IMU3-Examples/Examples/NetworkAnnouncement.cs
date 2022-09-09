using System;

namespace Ximu3Examples
{
    class NetworkAnnouncement
    {
        public NetworkAnnouncement()
        {
            Ximu3.NetworkAnnouncement networkAnnouncement = new Ximu3.NetworkAnnouncement();

            if (Helpers.YesOrNo("Use async implementation?") == true)
            {
                networkAnnouncement.NetworkAnnouncementEvent += NetworkAnnouncementEvent;
                Helpers.Wait(-1);
            }
            else
            {
                foreach (Ximu3.NetworkAnnouncementMessage message in networkAnnouncement.GetMessages())
                {
                    PrintMessage(message);
                }
            }
        }

        private void NetworkAnnouncementEvent(Object sender, Ximu3.NetworkAnnouncementEventArgs args)
        {
            PrintMessage(args.message);
        }

        private void PrintMessage(Ximu3.NetworkAnnouncementMessage message)
        {
            Console.WriteLine(message.DeviceName + " - " +
                message.SerialNumber + ", RSSI: " +
                message.Rssi + "%, Battery: " +
                message.Battery + "%, " +
                message.Status + ", " +
                message.TcpConnectionInfo + ", " +
                message.UdpConnectionInfo);
            // Console.WriteLine(message); // alternative to above
        }
    }
}
