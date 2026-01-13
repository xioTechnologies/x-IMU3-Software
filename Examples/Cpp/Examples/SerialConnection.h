#pragma once

#include "Connection.h"

class SerialConnection : public Connection {
public:
    SerialConnection() {
        if (helpers::yesOrNo("Search for connections?")) {
            const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeSerial);

            if (devices.empty()) {
                std::cout << "No serial connections available" << std::endl;
                return;
            }

            std::cout << "Found " << ximu3::XIMU3_device_to_string(devices[0]) << std::endl;

            const auto connectionInfo = ximu3::ConnectionInfo::from(devices[0]);

            run(*connectionInfo);
        } else {
            const ximu3::SerialConnectionInfo connectionInfo("COM1", 115200, false); // replace with actual connection info

            run(connectionInfo);
        }
    }
};
