#pragma once

#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include "Ximu3.hpp"

class PortScanner {
public:
    PortScanner() {
        if (helpers::yesOrNo("Use async implementation?")) {
            ximu3::PortScanner portScanner(callback);

            std::this_thread::sleep_for(std::chrono::seconds(60));
        } else {
            const auto devices = ximu3::PortScanner::scan();

            std::cout << "Found " << devices.size() << " devices" << std::endl;

            printDevices(devices);
        }
    }

private:
    std::function<void(const std::vector<ximu3::XIMU3_Device> &)> callback = [](const auto &devices) {
        std::cout << "Devices updated (" << devices.size() << " devices available)" << std::endl;
        printDevices(devices);
    };

    static void printDevices(const std::vector<ximu3::XIMU3_Device> &devices) {
        for (const auto &device: devices) {
            std::cout << device.device_name << ", " << device.serial_number << ", " << ximu3::ConnectionInfo::from(device)->toString() << std::endl;
            // std::cout << ximu3::XIMU3_device_to_string(device) << std::endl; // alternative to above
        }
    }
};
