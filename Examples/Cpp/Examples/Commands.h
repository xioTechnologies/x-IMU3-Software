#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <iostream>
#include <memory>

class Commands
{
public:
    Commands()
    {
        // Search for connection
        std::cout << "Searching for connections" << std::endl;
        const auto devices = ximu3::SerialDiscovery::scanFilter(2000, ximu3::XIMU3_ConnectionTypeUsb);
        if (devices.size() == 0)
        {
            std::cout << "No USB connections available" << std::endl;
            return;
        }
        std::cout << "Found " << devices[0].device_name << " - " << devices[0].serial_number << std::endl;

        // Open connection
        ximu3::Connection connection(ximu3::UsbConnectionInfo(devices[0].usb_connection_info));
        if (connection.open() != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Unable to open connection" << std::endl;
            return;
        }
        std::cout << "Connection successful" << std::endl;

        // Define read/write setting commands
        const std::vector<std::string> commands {
                "{\"deviceName\":null}", /* change null to a value to write setting */
                "{\"serialNumber\":null}",
                "{\"firmwareVersion\":null}",
                "{\"bootloaderVersion\":null}",
                "{\"hardwareVersion\":null}",
                "{\"invalidSettingKey\":null}", /* this command is deliberately invalid to demonstrate a failed command */
        };

        // Send commands
        if (helpers::yesOrNo("Use async implementation?") == true)
        {
            connection.sendCommandsAsync(commands, 2, 500, callback);
            helpers::wait(3);
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
