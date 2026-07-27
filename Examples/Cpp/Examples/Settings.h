#pragma once

#include <chrono>
#include <iostream>
#include <thread>
#include "Ximu3.hpp"

class Settings {
public:
    Settings() {
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

        // Backup settings
        const auto filePath = "C:/Users/Public/x-IMU3 Example Settings.json";

        const auto resultBackup = ximu3::settings_backup(filePath, connection);

        if (resultBackup != ximu3::XIMU3_ResultOk) {
            std::cout << "Backup failed. " << XIMU3_result_to_string(resultBackup) << "." << std::endl;
        }

        // Restore settings
        const auto resultRestore = ximu3::settings_restore(filePath, connection);

        if (resultRestore != ximu3::XIMU3_ResultOk) {
            std::cout << "Restore failed. " << XIMU3_result_to_string(resultRestore) << "." << std::endl;
        }

        // Save command
        connection.sendCommand("{\"save\":null}");

        // Close connection
        connection.close();
    }
};
