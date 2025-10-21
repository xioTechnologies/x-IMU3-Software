#pragma once

#include <chrono>
#include <iostream>
#include <memory>
#include <numeric>
#include <thread>
#include <span>
#include "Ximu3.hpp"

class SerialAccessoryLoopback
{
public:
    SerialAccessoryLoopback()
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

        // Loopback
        std::function callback = [&] (ximu3::XIMU3_SerialAccessoryMessage message)
        {
            printf("RX ");
            printBytes({reinterpret_cast<std::uint8_t*>(message.char_array), message.number_of_bytes});
            printf("\n");
        };

        connection.addSerialAccessoryCallback(callback);

        std::vector<std::uint8_t> bytes(32);

        for (int value = 0; value < 256; value += static_cast<int>(bytes.size()))
        {
            std::iota(bytes.begin(), bytes.end(), static_cast<std::uint8_t>(value));

            printf("TX ");
            printBytes(bytes);

            connection.sendCommands({"{ \"accessory\": " + std::string(ximu3::XIMU3_bytes_to_json_string(bytes.data(), bytes.size())) + " }"}, 2, 500);

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        // Close connection
        connection.close();
    }

private:
    static void printBytes(const std::span<const std::uint8_t> bytes)
    {
        for (const auto byte : bytes)
        {
            printf("%02X ", byte);
        }
        printf("\n");
    }
};
