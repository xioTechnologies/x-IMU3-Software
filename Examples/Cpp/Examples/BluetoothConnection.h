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

            const auto connectionInfo = ximu3::ConnectionInfo::from(devices[0]);

            run(*connectionInfo);
        } else {
            const ximu3::BluetoothConnectionInfo connectionInfo("COM1"); // replace with actual connection info

            run(connectionInfo);
        }
    }
};
