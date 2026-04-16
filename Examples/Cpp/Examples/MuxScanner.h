#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include "Ximu3.hpp"

class MuxScanner {
public:
    MuxScanner() {
        // Search for connection
        const auto usbDevices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for OS to release port

        if (usbDevices.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << ximu3::XIMU3_device_to_string(usbDevices[0]) << std::endl;

        // Open connection
        ximu3::Connection usbConnection(*ximu3::ConnectionConfig::from(usbDevices[0]));

        const auto result = usbConnection.open();

        if (result != ximu3::XIMU3_ResultOk) {
            std::cout << "Unable to open " << usbConnection.getConfig()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        // Mux scanner (blocking)
        const auto muxDevices = ximu3::MuxScanner::scan(usbConnection);

        printDevices(muxDevices);

        // Mux scanner (non-blocking)
        const ximu3::MuxScanner muxScanner(usbConnection, callback);

        std::this_thread::sleep_for(std::chrono::seconds(60));

        // Close connection
        usbConnection.close();
    }

private:
    std::function<void(const std::vector<ximu3::XIMU3_Device> &)> callback = [](const auto &devices) {
        printDevices(devices);
    };

    static void printDevices(const std::vector<ximu3::XIMU3_Device> &devices) {
        std::cout << devices.size() << " device(s) found" << std::endl;

        for (const auto &device: devices) {
            std::cout << device.device_name << ", " << device.serial_number << ", " << ximu3::ConnectionConfig::from(device)->toString() << std::endl;
            // std::cout << ximu3::XIMU3_device_to_string(device) << std::endl; // alternative to above
        }
    }
};
