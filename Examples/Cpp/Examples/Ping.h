#pragma once

#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "Ximu3.hpp"

class Ping {
public:
    Ping() {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        if (devices.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << ximu3::XIMU3_device_to_string(devices[0]) << std::endl;

        // Open connection
        ximu3::Connection connection(*ximu3::ConnectionConfig::from(devices[0]));

        const auto result = connection.open();

        if (result != ximu3::XIMU3_ResultOk) {
            std::cout << "Unable to open " << connection.getConfig()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        // Ping
        if (helpers::yesOrNo("Use async implementation?")) {
            connection.pingAsync(callback);

            std::this_thread::sleep_for(std::chrono::seconds(3));
        } else {
            printResponse(connection.ping());
        }

        // Close connection
        connection.close();
    }

private:
    std::function<void(std::optional<ximu3::XIMU3_PingResponse>)> callback = [](const auto response) {
        printResponse(response);
    };

    static void printResponse(const std::optional<ximu3::XIMU3_PingResponse> &response) {
        if (response.has_value() == false) {
            std::cout << "No response" << std::endl;
            return;
        }

        std::cout << response->interface << ", " << response->device_name << ", " << response->serial_number << std::endl;
        // std::cout << ximu3::XIMU3_ping_response_to_string(*response) << std::endl; // alternative to above
    }
};
