#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include "Ximu3.hpp"

class PortScanner {
public:
    PortScanner() {
        // Blocking
        const auto devices = ximu3::PortScanner::scan();

        printDevices(devices);

        // Non-blocking
        ximu3::PortScanner portScanner(callback);

        std::this_thread::sleep_for(std::chrono::seconds(60));
    }

private:
    std::function<void(const std::vector<ximu3::Device> &)> callback = [](const auto &devices) {
        printDevices(devices);
    };

    static void printDevices(const std::vector<ximu3::Device> &devices) {
        std::cout << devices.size() << " device(s) found" << std::endl;

        for (const auto &device: devices) {
            std::cout << device.device_name << ", " << device.serial_number << ", " << ximu3::ConnectionConfig::from(device)->toString() << std::endl;
            // std::cout << device.toString() << std::endl; // alternative to above
        }
    }
};
