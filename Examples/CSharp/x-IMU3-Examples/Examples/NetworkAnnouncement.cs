using System;

namespace Ximu3Examples
{
    class NetworkAnnouncement
    {
        public NetworkAnnouncement()
        {
            Ximu3.NetworkAnnouncement networkAnnouncement = new Ximu3.NetworkAnnouncement();

            if (networkAnnouncement.GetResult() != Ximu3.Result.Ok)
            {
                Console.WriteLine("Unable to open network announcement socket");
                return;
            }

            if (Helpers.AskQuestion("Use async implementation?"))
            {
                networkAnnouncement.NetworkAnnouncementEvent += NetworkAnnouncementEvent;
                System.Threading.Thread.Sleep(60000);
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
            Console.WriteLine(message.DeviceName + ", " +
                message.SerialNumber + ", " +
                message.IPAddress + ", " +
                message.TcpPort + ", " +
                message.UdpSend + ", " +
                message.UdpReceive + ", " +
                message.Rssi + "%, " +
                message.Battery + "%, " +
                message.ChargingStatus);
            // Console.WriteLine(message); // alternative to above
        }
    }
}
