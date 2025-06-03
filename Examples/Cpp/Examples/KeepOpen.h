#pragma once

#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

class KeepOpen
{
public:
    KeepOpen()
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
        ximu3::Connection connection(*ximu3::connectionInfoFrom(devices[0]));

        const ximu3::KeepOpen keepOpen(connection, callback);

        // Close connection
        std::this_thread::sleep_for(std::chrono::seconds(60));
        connection.close();
    }

private:
    std::function<void(const ximu3::XIMU3_ConnectionStatus)> callback = [](const auto status)
    {
        switch (status)
        {
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
