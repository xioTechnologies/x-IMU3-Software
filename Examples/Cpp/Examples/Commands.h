#pragma once

#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "Ximu3.hpp"

class Commands {
public:
    Commands() {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        if (devices.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << ximu3::XIMU3_device_to_string(devices[0]) << std::endl;

        // Open connection
        ximu3::Connection connection(*ximu3::ConnectionInfo::from(devices[0]));

        const auto result = connection.open();

        if (result != ximu3::XIMU3_ResultOk) {
            std::cout << "Unable to open " << connection.getInfo()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        // Example commands
        const std::vector<std::string> commands{
            "{\"device_name\":\"Foobar\"}", // write "Foobar" to device name
            "{\"serial_number\":null}", // read serial number
            "{\"firmware_version\":null}", // read firmware version
            "{\"invalid_key\":null}", // invalid key to demonstrate an error response
        };

        // Send commands
        if (helpers::yesOrNo("Use async implementation?")) {
            connection.sendCommandsAsync(commands, callback);

            std::this_thread::sleep_for(std::chrono::seconds(3));
        } else {
            printResponses(connection.sendCommands(commands));
        }

        // Close connection
        connection.close();
    }

private:
    std::function<void(const std::vector<std::optional<ximu3::CommandMessage> > &)> callback = [](const auto &responses) {
        printResponses(responses);
    };

    static void printResponses(const std::vector<std::optional<ximu3::CommandMessage> > &responses) {
        for (const auto &response: responses) {
            if (response.has_value() == false) {
                std::cout << "No response" << std::endl;
                continue;
            }

            if (const auto error = response->error) {
                std::cout << *error << std::endl;
                continue;
            }

            std::cout << response->key << " : " << response->value << std::endl;
        }
    }
};
