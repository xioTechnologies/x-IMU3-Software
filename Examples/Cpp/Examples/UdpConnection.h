#pragma once

#include "Connection.h"

class UdpConnection : public Connection
{
public:
    UdpConnection()
    {
        if (helpers::yesOrNo("Search for connections?"))
        {
            const auto messages = ximu3::NetworkAnnouncement().getMessagesAfterShortDelay();

            if (messages.empty())
            {
                std::cout << "No UDP connections available" << std::endl;
                return;
            }

            std::cout << "Found " << messages[0].device_name << " " << messages[0].serial_number << std::endl;

            const ximu3::UdpConnectionInfo connectionInfo(messages[0]);

            run(connectionInfo);
        }
        else
        {
            const ximu3::UdpConnectionInfo connectionInfo("192.168.1.1", 9000, 8000); // replace with actual connection info

            run(connectionInfo);
        }
    }
};
