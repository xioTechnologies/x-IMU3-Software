#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include "Ximu3.hpp"

class OpenAsync {
public:
    OpenAsync() {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        if (devices.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << ximu3::XIMU3_device_to_string(devices[0]) << std::endl;

        // Open connection
        ximu3::Connection connection(*ximu3::ConnectionInfo::from(devices[0]));

        const auto callback = [&connection](const auto result) {
            if (result != ximu3::XIMU3_ResultOk) {
                std::cout << "Unable to open " << connection.getInfo()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
                return;
            }

            std::cout << XIMU3_result_to_string(result) << std::endl;
        };

        connection.openAsync(callback);

        // Close connection
        std::this_thread::sleep_for(std::chrono::seconds(3));

        connection.close();
    }
};
