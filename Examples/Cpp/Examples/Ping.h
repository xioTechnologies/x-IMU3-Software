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
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_ConnectionTypeUsb);

        if (devices.empty())
        {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << devices[0].device_name << " " << devices[0].serial_number << std::endl;

        // Open connection
        ximu3::Connection connection(ximu3::UsbConnectionInfo(devices[0].usb_connection_info));

        if (connection.open() != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Unable to open connection" << std::endl;
            return;
        }

        // Ping
        const auto response = connection.ping();

        if (response.result == ximu3::XIMU3_ResultOk)
        {
            std::cout << response.interface << ", " << response.device_name << ", " << response.serial_number << std::endl;
            // std::cout << ximu3::XIMU3_ping_response_to_string(response) << std::endl; // alternative to above
        }
        else
        {
            std::cout << "No response" << std::endl;
        }

        // Close connection
        connection.close();
    }
};
