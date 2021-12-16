#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <iostream>

class SerialDiscovery
{
public:
    SerialDiscovery()
    {
        if (helpers::yesOrNo("Use async implementation?") == true)
        {
            ximu3::SerialDiscovery discovery(callback);
            helpers::wait(-1);
        }
        else
        {
            const auto devices = ximu3::SerialDiscovery::scan(2000);
            std::cout << "Discovered " << devices.size() << " devices" << std::endl;
            printDevices(devices);
        }
    }

private:
    std::function<void(const std::vector<ximu3::XIMU3_DiscoveredSerialDevice>&)> callback = [](const auto& devices)
    {
        std::cout << "Devices updated (" << devices.size() << " devices available)" << std::endl;
        printDevices(devices);
    };

    static void printDevices(const std::vector<ximu3::XIMU3_DiscoveredSerialDevice>& devices)
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
            std::cout << device.device_name << " - " << device.serial_number << ", " << connectionInfo << std::endl;
            // std::cout << XIMU3_discovered_serial_device_to_string(*device) << std::endl; // alternative to above
        }
    }
};
