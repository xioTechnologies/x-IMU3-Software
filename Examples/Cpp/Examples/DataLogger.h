#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
#include <vector>
#include "Ximu3.hpp"

class DataLogger {
public:
    DataLogger() {
        // Open all USB connections
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for OS to release port

        std::vector<std::unique_ptr<ximu3::Connection> > connections;

        for (auto device: devices) {
            std::cout << "Found " << ximu3::XIMU3_device_to_string(device) << std::endl;

            auto connection = std::make_unique<ximu3::Connection>(*ximu3::ConnectionConfig::from(device));

            const auto result = connection->open();

            if (result == ximu3::XIMU3_ResultOk) {
                connections.push_back(std::move(connection));
            } else {
                std::cout << "Unable to open " << connection->getConfig()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
            }
        }

        if (connections.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        // Log data (blocking)
        const auto destination = "C:/";
        const auto nameBlocking = "x-IMU3 Data Logger Example Blocking";

        const auto resultBlocking = ximu3::DataLogger::log(destination, nameBlocking, toRawPointers(connections), 3);

        if (resultBlocking != ximu3::XIMU3_ResultOk) {
            std::cout << "Data logger failed. " << XIMU3_result_to_string(resultBlocking) << "." << std::endl;
        }

        // Log data (non-blocking)
        const auto nameNonBlocking = "x-IMU3 Data Logger Example Non-Blocking";

        auto dataLogger = std::make_unique<ximu3::DataLogger>(destination, nameNonBlocking, toRawPointers(connections));

        const auto resultNonBlocking = dataLogger->getResult();

        if (resultNonBlocking != ximu3::XIMU3_ResultOk) {
            std::cout << "Data logger failed. " << XIMU3_result_to_string(resultNonBlocking) << "." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));

        dataLogger.reset(); // stop logging

        std::cout << "Complete" << std::endl;

        // Close all connections
        for (auto &connection: connections) {
            connection->close();
        }
    }

private:
    static std::vector<ximu3::Connection *> toRawPointers(const std::vector<std::unique_ptr<ximu3::Connection> > &connections) {
        std::vector<ximu3::Connection *> rawPointers;

        for (auto &connection: connections) {
            rawPointers.push_back(connection.get());
        }

        return rawPointers;
    }
};
