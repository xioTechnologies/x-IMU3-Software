#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <thread>

class PortScanner
{
public:
    PortScanner()
    {
        if (helpers::askQuestion("Use async implementation?"))
        {
            ximu3::PortScanner portScanner(callback);
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
        else
        {
            const auto devices = ximu3::PortScanner::scan();
            std::cout << "Found " << devices.size() << " devices" << std::endl;
            printDevices(devices);
        }
    }

private:
    std::function<void(const std::vector<ximu3::XIMU3_Device>&)> callback = [](const auto& devices)
    {
        std::cout << "Devices updated (" << devices.size() << " devices available)" << std::endl;
        printDevices(devices);
    };

    static void printDevices(const std::vector<ximu3::XIMU3_Device>& devices)
    {
        for (const auto& device : devices)
        {
            const char* connectionInfo;
            switch (device.connection_type)
            {
                case ximu3::XIMU3_ConnectionTypeUsb:
                    connectionInfo = XIMU3_usb_connection_info_to_string(device.usb_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeSerial:
                    connectionInfo = XIMU3_serial_connection_info_to_string(device.serial_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeBluetooth:
                    connectionInfo = XIMU3_bluetooth_connection_info_to_string(device.bluetooth_connection_info);
                    break;
                default:
                    connectionInfo = "";
                    break;
            }
            std::cout << device.device_name << ", " << device.serial_number << ", " << connectionInfo << std::endl;
            // std::cout << XIMU3_device_to_string(*device) << std::endl; // alternative to above
        }
    }
};
