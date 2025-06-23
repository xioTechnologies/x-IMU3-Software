#pragma once

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include "Ximu3.hpp"

class Demux
{
public:
    Demux()
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

        // Demux
        {
            const std::vector<std::uint8_t> channels { 0x41, 0x42, 0x43 };

            ximu3::Demux demux(connection, channels);

            const auto result = demux.getResult();

            printResult(result);

            if (result == ximu3::XIMU3_ResultOk)
            {
                std::this_thread::sleep_for(std::chrono::seconds(60));
            }
        }

        // Close connection
        connection.close();
    }

private:
    static void printResult(const ximu3::XIMU3_Result result)
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
        // std::cout << ximu3::XIMU3_result_to_string(result) << std::endl; // alternative to above
    }
};
