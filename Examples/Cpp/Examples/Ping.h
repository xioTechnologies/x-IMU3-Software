#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

class Ping
{
public:
    Ping()
    {
        // Create connection info
        const ximu3::UsbConnectionInfo connectionInfo("COM1");

        // Open and ping
        connection = std::make_unique<ximu3::Connection>(connectionInfo);
        if (helpers::askQuestion("Use async implementation?"))
        {
            connection->openAsync(callback);
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        else
        {
            if (connection->open() != ximu3::XIMU3_ResultOk)
            {
                std::cout << "Unable to open connection" << std::endl;
                return;
            }
            printPingResponse(connection->ping());
        }

        // Close connection
        connection->close();
    }

private:
    std::function<void(ximu3::XIMU3_Result)> callback = [&](auto result)
    {
        if (result != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Unable to open connection" << std::endl;
            return;
        }
        printPingResponse(connection->ping());
    };

    std::unique_ptr<ximu3::Connection> connection; // declare connection after callbacks so that it is destroyed first

    static void printPingResponse(const ximu3::XIMU3_PingResponse pingResponse)
    {
        if (pingResponse.result != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Ping failed" << std::endl;
            return;
        }
        std::cout << pingResponse.interface << ", " << pingResponse.device_name << ", " << pingResponse.serial_number << std::endl;
        // std::cout << ximu3::XIMU3_ping_response_to_string(pingResponse) << std::endl; // alternative to above
    }
};
