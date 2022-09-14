#include "Connection.h"

class TcpConnection : public Connection
{
public:
    TcpConnection()
    {
        if (helpers::yesOrNo("Search for connections?") == true)
        {
            std::cout << "Searching for connections" << std::endl;
            const auto messages = ximu3::NetworkAnnouncement().getMessagesAfterShortDelay();
            if (messages.size() == 0)
            {
                std::cout << "No TCP connections available" << std::endl;
                return;
            }
            std::cout << "Found " << messages[0].device_name << " - " << messages[0].serial_number << std::endl;
            run(ximu3::TcpConnectionInfo(messages[0].tcp_connection_info));
        }
        else
        {
            run(ximu3::TcpConnectionInfo("192.168.1.1", 7000));
        }
    }
};
