#pragma once

#include "../Helpers.hpp"
#include "Connection.h"
#include <iostream>
#include "Ximu3.hpp"

class UdpConnection : public Connection {
public:
    UdpConnection() {
        if (helpers::yesOrNo("Connect automatically?")) {
            ximu3::NetworkAnnouncement networkAnnouncement;

            const auto result = networkAnnouncement.getResult();

            if (result != ximu3::XIMU3_ResultOk) {
                std::cout << "Network announcement failed: " << XIMU3_result_to_string(result) << std::endl;
                return;
            }

            const auto messages = networkAnnouncement.getMessagesAfterShortDelay();

            if (messages.empty()) {
                std::cout << "No network connections available" << std::endl;
                return;
            }

            const auto config = ximu3::UdpConnectionConfig::from(messages.front());

            if (config == nullptr) {
                std::cout << "No UDP connections available" << std::endl;
                return;
            }

            std::cout << "Found " << messages[0].device_name << " " << messages[0].serial_number << std::endl;

            run(*config);
        } else {
            const ximu3::UdpConnectionConfig config("192.168.1.1", 9000, 8000); // replace with actual connection config

            run(config);
        }
    }
};
