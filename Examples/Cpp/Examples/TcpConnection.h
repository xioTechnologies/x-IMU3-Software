#pragma once

#include "Connection.h"

class TcpConnection : public Connection
{
public:
    TcpConnection()
    {
        if (helpers::yesOrNo("Search for connections?"))
        {
            const auto messages = ximu3::NetworkAnnouncement().getMessagesAfterShortDelay();

            if (messages.empty())
            {
                std::cout << "No TCP connections available" << std::endl;
                return;
            }

            std::cout << "Found " << messages[0].device_name << " " << messages[0].serial_number << std::endl;

            const ximu3::TcpConnectionInfo connectionInfo(messages[0]);

            run(connectionInfo);
        }
        else
        {
            const ximu3::TcpConnectionInfo connectionInfo("192.168.1.1", 7000); // replace with actual connection info

            run(connectionInfo);
        }
    }
};
