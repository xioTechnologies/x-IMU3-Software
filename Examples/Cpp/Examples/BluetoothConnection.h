#pragma once

#include "Connection.h"

class BluetoothConnection : public Connection {
public:
    BluetoothConnection() {
        if (helpers::yesOrNo("Search for connections?")) {
            const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeBluetooth);

            if (devices.empty()) {
                std::cout << "No Bluetooth connections available" << std::endl;
                return;
            }

            std::cout << "Found " << ximu3::XIMU3_device_to_string(devices[0]) << std::endl;

            const auto config = ximu3::ConnectionConfig::from(devices[0]);

            run(*config);
        } else {
            const ximu3::BluetoothConnectionConfig config("COM1"); // replace with actual connection config

            run(config);
        }
    }
};
