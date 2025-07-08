#pragma once

#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

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

        if (connection.open() != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Unable to open connection" << std::endl;
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
    std::function<void(const ximu3::XIMU3_PingResponse)> callback = [](const auto pingResponse)
    {
        printPingResponse(pingResponse);
    };

    static void printPingResponse(const ximu3::XIMU3_PingResponse& pingResponse)
    {
        if (pingResponse.result == ximu3::XIMU3_ResultOk)
        {
            std::cout << pingResponse.interface << ", " << pingResponse.device_name << ", " << pingResponse.serial_number << std::endl;
            // std::cout << ximu3::XIMU3_ping_response_to_string(pingResponse) << std::endl; // alternative to above
        }
        else
        {
            std::cout << "No response" << std::endl;
        }
    }
};
