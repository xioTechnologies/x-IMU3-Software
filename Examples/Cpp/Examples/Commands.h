#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

class Commands
{
public:
    Commands()
    {
        // Search for connection
        std::cout << "Searching for connections" << std::endl;
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_ConnectionTypeUsb);
        if (devices.size() == 0)
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

        // Define read/write setting commands
        const std::vector<std::string> commands {
                "{\"device_name\":null}", /* change null to a value to write setting */
                "{\"serial_number\":null}",
                "{\"firmware_version\":null}",
                "{\"bootloader_version\":null}",
                "{\"hardware_version\":null}",
                "{\"invalid_setting_key\":null}", /* invalid key to demonstrate an error response */
        };

        // Send commands
        if (helpers::askQuestion("Use async implementation?"))
        {
            connection.sendCommandsAsync(commands, 2, 500, callback);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        else
        {
            printResponses(connection.sendCommands(commands, 2, 500));
        }

        // Close connection
        connection.close();
    }

private:
    std::function<void(const std::vector<std::string>&)> callback = [](const auto& responses)
    {
        printResponses(responses);
    };

    static void printResponses(const std::vector<std::string>& responses)
    {
        std::cout << responses.size() << " commands confirmed" << std::endl;
        for (const auto& response : responses)
        {
            std::cout << response.c_str() << std::endl;
        }
    }
};
