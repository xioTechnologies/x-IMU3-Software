#pragma once

#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

class OpenAsync
{
public:
    OpenAsync()
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

        connection.openAsync(callback);

        // Close connection
        std::this_thread::sleep_for(std::chrono::seconds(3));
        connection.close();
    }

private:
    std::function<void(const ximu3::XIMU3_Result)> callback = [](const auto result)
    {
        switch (result)
        {
            case ximu3::XIMU3_ResultOk:
                std::cout << "OK" << std::endl;
                break;
            case ximu3::XIMU3_ResultError:
                std::cout << "Error" << std::endl;
                break;
        }
        // std::cout << XIMU3_result_to_string(result) << std::endl; // alternative to above
    };
};
