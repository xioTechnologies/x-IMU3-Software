#include "Connection.h"

class UdpConnection : public Connection
{
public:
    UdpConnection()
    {
        if (helpers::askQuestion("Search for connections?"))
        {
            std::cout << "Searching for connections" << std::endl;
            const auto messages = ximu3::NetworkAnnouncement().getMessagesAfterShortDelay();
            if (messages.size() == 0)
            {
                std::cout << "No UDP connections available" << std::endl;
                return;
            }
            std::cout << "Found " << messages[0].device_name << " - " << messages[0].serial_number << std::endl;
            run(ximu3::UdpConnectionInfo(ximu3::XIMU3_network_announcement_message_to_udp_connection_info(messages[0])));
        }
        else
        {
            run(ximu3::UdpConnectionInfo("192.168.1.1", 9000, 8000));
        }
    }
};
