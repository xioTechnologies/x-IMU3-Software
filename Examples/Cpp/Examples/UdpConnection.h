#pragma once

#include "Connection.h"

class UdpConnection : public Connection {
public:
    UdpConnection() {
        if (helpers::yesOrNo("Search for connections?")) {
            ximu3::NetworkAnnouncement networkAnnouncement;

            const auto result = networkAnnouncement.getResult();

            if (result != ximu3::XIMU3_ResultOk) {
                std::cout << "Network announcement failed. " << XIMU3_result_to_string(result) << "." << std::endl;
                return;
            }

            const auto messages = networkAnnouncement.getMessagesAfterShortDelay();

            if (messages.empty()) {
                std::cout << "No UDP connections available" << std::endl;
                return;
            }

            std::cout << "Found " << messages[0].device_name << " " << messages[0].serial_number << std::endl;

            const ximu3::UdpConnectionInfo connectionInfo(messages[0]);

            run(connectionInfo);
        } else {
            const ximu3::UdpConnectionInfo connectionInfo("192.168.1.1", 9000, 8000); // replace with actual connection info

            run(connectionInfo);
        }
    }
};
