#pragma once

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <ios>
#include <thread>
#include <span>
#include <string>
#include <thread>
#include "Ximu3.hpp"

class SerialAccessory {
public:
    SerialAccessory() {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for OS to release port

        if (devices.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << devices[0].toString() << std::endl;

        // Open connection
        ximu3::Connection connection(*ximu3::ConnectionConfig::from(devices[0]));

        const auto result = connection.open();

        if (result != ximu3::XIMU3_ResultOk) {
            std::cout << "Unable to open " << connection.getConfig()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        // Send accessory data
        const std::uint8_t data[] = "Any value \x00 to \xFF\n";
        const auto dataLength = sizeof(data) - 1; // exclude string termination

        const std::string data_as_json = ximu3::XIMU3_bytes_to_json(data, dataLength);

        const auto response = connection.sendCommand("{\"accessory\":" + data_as_json + "}");

        if (response.has_value() == false) {
            std::cout << "No response" << std::endl;
        } else if (const auto error = response->error) {
            std::cout << *error << std::endl;
        } else {
            printData("TX", {data, dataLength});
        }

        // Receive accessory data
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (const auto message = connection.getSerialAccessoryMessage()) {
            printData("RX", {reinterpret_cast<const std::uint8_t *>(message->char_array), message->number_of_bytes});
        }

        // Close connection
        connection.close();
    }

private:
    static void printData(const char *const direction, const std::span<const std::uint8_t> data) {
        std::cout << direction << " ";

        for (const auto byte: data) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << byte << " ";
        }

        std::cout << std::endl;
    }
};
