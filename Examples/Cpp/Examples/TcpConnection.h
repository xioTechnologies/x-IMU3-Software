#pragma once

#include "Connection.h"

class TcpConnection : public Connection {
public:
    TcpConnection() {
        if (helpers::yesOrNo("Search for connections?")) {
            ximu3::NetworkAnnouncement networkAnnouncement;

            const auto result = networkAnnouncement.getResult();

            if (result != ximu3::XIMU3_ResultOk) {
                std::cout << "Network announcement failed. " << XIMU3_result_to_string(result) << "." << std::endl;
                return;
            }

            const auto messages = networkAnnouncement.getMessagesAfterShortDelay();

            if (messages.empty()) {
                std::cout << "No TCP connections available" << std::endl;
                return;
            }

            std::cout << "Found " << messages[0].device_name << " " << messages[0].serial_number << std::endl;

            const ximu3::TcpConnectionInfo connectionInfo(messages[0]);

            run(connectionInfo);
        } else {
            const ximu3::TcpConnectionInfo connectionInfo("192.168.1.1", 7000); // replace with actual connection info

            run(connectionInfo);
        }
    }
};
