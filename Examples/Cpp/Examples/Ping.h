#pragma once

#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "Ximu3.hpp"

class Ping
{
public:
    Ping()
    {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        if (devices.empty())
        {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << devices[0].device_name << " " << devices[0].serial_number << std::endl;

        // Open connection
        ximu3::Connection connection(*ximu3::ConnectionInfo::from(devices[0]));

        const auto result = connection.open();

        if (result != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Unable to open connection. " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        // Ping
        if (helpers::yesOrNo("Use async implementation?"))
        {
            connection.pingAsync(callback);

            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        else
        {
            printPingResponse(connection.ping());
        }

        // Close connection
        connection.close();
    }

private:
    std::function<void(std::optional<ximu3::XIMU3_PingResponse>)> callback = [](const auto pingResponse)
    {
        printPingResponse(pingResponse);
    };

    static void printPingResponse(const std::optional<ximu3::XIMU3_PingResponse>& pingResponse)
    {
        if (pingResponse.has_value() == false)
        {
            printf("No response");
            return;
        }

        std::cout << pingResponse->interface << ", " << pingResponse->device_name << ", " << pingResponse->serial_number << std::endl;
        // std::cout << ximu3::XIMU3_ping_response_to_string(*pingResponse) << std::endl; // alternative to above
    }
};
