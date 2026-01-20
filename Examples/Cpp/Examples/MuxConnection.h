#pragma once

#include "Connection.h"

class MuxConnection : public Connection {
public:
    MuxConnection() {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        if (devices.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << ximu3::XIMU3_device_to_string(devices[0]) << std::endl;

        // Open connection
        ximu3::Connection usbConnection(*ximu3::ConnectionConfig::from(devices[0]));

        const auto result = usbConnection.open();

        if (result != ximu3::XIMU3_ResultOk) {
            std::cout << "Unable to open " << usbConnection.getConfig()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        ximu3::MuxConnectionConfig config(0x41, usbConnection);

        run(config);
    }
};
