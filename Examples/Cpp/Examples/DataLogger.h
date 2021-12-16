#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <functional>
#include <iostream>

class DataLogger
{
public:
    DataLogger()
    {
        // Open all USB connections
        const auto devices = ximu3::SerialDiscovery::scanFilter(2000, ximu3::XIMU3_ConnectionTypeUsb);
        std::vector<std::unique_ptr<ximu3::Connection>> connections;
        for (auto device : devices)
        {
            std::cout << XIMU3_discovered_serial_device_to_string(device) << std::endl;
            auto connection = std::make_unique<ximu3::Connection>(ximu3::UsbConnectionInfo(device.usb_connection_info));
            if (connection->open() == ximu3::XIMU3_ResultOk)
            {
                connections.push_back(std::move(connection));
            }
            else
            {
                std::cout << "Unable to open connection" << std::endl;
            }
        }

        // Log data
        const auto directory = "C:/";
        const auto name = "Data Logger Example";
        if (helpers::yesOrNo("Use async implementation?") == true)
        {
            auto dataLogger = ximu3::DataLogger(directory, name, toRawPointers(connections), callback);
            helpers::wait(3);
        }
        else
        {
            printResult(ximu3::DataLogger::log(directory, name, toRawPointers(connections), 3));
        }

        // Close all connections
        for (auto& connection : connections)
        {
            connection->close();
        }
    }

private:
    static std::vector<ximu3::Connection*> toRawPointers(const std::vector<std::unique_ptr<ximu3::Connection>>& connections)
    {
        std::vector<ximu3::Connection*> rawPointers;
        for (auto& connection : connections)
        {
            rawPointers.push_back(connection.get());
        }
        return rawPointers;
    }

    std::function<void(const ximu3::XIMU3_Result)> callback = [](const auto& result)
    {
        printResult(result);
    };

    static void printResult(const ximu3::XIMU3_Result result)
    {
        switch (result)
        {
            case ximu3::XIMU3_ResultOk:
                std::cout << "Ok" << std::endl;
                break;
            case ximu3::XIMU3_ResultError:
                std::cout << "Error" << std::endl;
                break;
        }
        // std::cout << ximu3::XIMU3_result_to_string(result) << std::endl; // alternative to above
    }
};
