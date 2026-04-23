#pragma once

#include "../Helpers.hpp"
#include <chrono>
#include "Connection.h"
#include <iostream>
#include <thread>
#include "Ximu3.hpp"

class MuxConnection : public Connection {
public:
    MuxConnection() {
        // Search for connection
        const auto usbDevices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for OS to release port

        if (usbDevices.empty()) {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << usbDevices[0].toString() << std::endl;

        // Open connection
        ximu3::Connection usbConnection(*ximu3::ConnectionConfig::from(usbDevices[0]));

        const auto result = usbConnection.open();

        if (result != ximu3::XIMU3_ResultOk) {
            std::cout << "Unable to open " << usbConnection.getConfig()->toString() << ". " << XIMU3_result_to_string(result) << "." << std::endl;
            return;
        }

        // Mux connection
        if (helpers::yesOrNo("Connect automatically?")) {
            const auto muxDevices = ximu3::MuxScanner::scan(usbConnection);

            if (muxDevices.empty()) {
                std::cout << "No mux connections available" << std::endl;
                return;
            }

            std::cout << "Found " << muxDevices[0].toString() << std::endl;

            const auto config = ximu3::ConnectionConfig::from(muxDevices[0]);

            run(*config);
        } else {
            const ximu3::MuxConnectionConfig config(0x5F, usbConnection); // replace with actual connection config

            run(config);
        }

        // Close connection
        usbConnection.close();
    }
};
