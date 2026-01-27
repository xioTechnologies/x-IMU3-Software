#pragma once

#include "Connection.h"

class SerialConnection : public Connection {
public:
    SerialConnection() {
        if (helpers::yesOrNo("Search for connections?")) {
            const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeSerial);

            std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for OS to release port

            if (devices.empty()) {
                std::cout << "No serial connections available" << std::endl;
                return;
            }

            std::cout << "Found " << ximu3::XIMU3_device_to_string(devices[0]) << std::endl;

            const auto config = ximu3::ConnectionConfig::from(devices[0]);

            run(*config);
        } else {
            const ximu3::SerialConnectionConfig config("COM1", 115200, false); // replace with actual connection config

            run(config);
        }
    }
};
