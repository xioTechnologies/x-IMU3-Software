#include "Connection.h"

class UdpConnection : public Connection
{
public:
    UdpConnection()
    {
        if (helpers::yesOrNo("Search for connections?") == true)
        {
            std::cout << "Searching for connections" << std::endl;
            const auto messages = ximu3::NetworkAnnouncement().getMessagesAfterShortDelay();
            if (messages.size() == 0)
            {
                std::cout << "No UDP connections available" << std::endl;
                return;
            }
            std::cout << "Found " << messages[0].device_name << " - " << messages[0].serial_number << std::endl;
            run(ximu3::UdpConnectionInfo(messages[0].udp_connection_info));
        }
        else
        {
            run(ximu3::UdpConnectionInfo("192.168.1.1", 9000, 8000));
        }
    }
};
