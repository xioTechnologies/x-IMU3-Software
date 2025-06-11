#pragma once

#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

class Commands
{
public:
    Commands()
    {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_ConnectionTypeUsb);

        if (devices.empty())
        {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << devices[0].device_name << " " << devices[0].serial_number << std::endl;

        // Open connection
        ximu3::Connection connection(*ximu3::ConnectionInfo::from(devices[0]));

        if (connection.open() != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Unable to open connection" << std::endl;
            return;
        }

        // Example commands
        const std::vector<std::string> commands {
            "{\"device_name\":\"Foobar\"}", // write "Foobar" to device name
            "{\"serial_number\":null}", // read serial number
            "{\"firmware_version\":null}", // read firmware version
            "{\"invalid_key\":null}", // invalid key to demonstrate an error response
        };

        // Send commands
        if (helpers::yesOrNo("Use async implementation?"))
        {
            connection.sendCommandsAsync(commands, 2, 500, callback);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        else
        {
            printResponses(connection.sendCommands(commands, 2, 500));
        }

        // Close connection
        connection.close();
    }

private:
    std::function<void(const std::vector<std::string>&)> callback = [](const auto& responses)
    {
        printResponses(responses);
    };

    static void printResponses(const std::vector<std::string>& responses)
    {
        std::cout << responses.size() << " responses" << std::endl;

        for (const auto& response_ : responses)
        {
            const auto response = ximu3::XIMU3_command_message_parse(response_.c_str());

            if (std::strlen(response.error) > 0)
            {
                std::cout << response.error << std::endl;
                return;
            }

            std::cout << response.key << " : " << response.value << std::endl;
        }
    }
};
