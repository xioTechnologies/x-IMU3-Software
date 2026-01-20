#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include "Ximu3.hpp"

class KeepOpen {
public:
    KeepOpen() {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        if (devices.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << ximu3::XIMU3_device_to_string(devices[0]) << std::endl;

        // Open connection
        ximu3::Connection connection(*ximu3::ConnectionInfo::from(devices[0]));

        const ximu3::KeepOpen keepOpen(connection, callback);

        // Close connection
        std::this_thread::sleep_for(std::chrono::seconds(60));

        connection.close();
    }

private:
    std::function<void(const ximu3::XIMU3_ConnectionStatus)> callback = [](const auto status) {
        switch (status) {
            case ximu3::XIMU3_ConnectionStatusConnected:
                std::cout << "Connected" << std::endl;
                break;
            case ximu3::XIMU3_ConnectionStatusReconnecting:
                std::cout << "Reconnecting" << std::endl;
                break;
        }
        // std::cout << XIMU3_connection_status_to_string(status) << std::endl; // alternative to above
    };
};
