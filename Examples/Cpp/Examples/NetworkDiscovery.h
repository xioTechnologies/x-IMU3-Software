#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <iostream>

class NetworkDiscovery
{
public:
    NetworkDiscovery()
    {
        if (helpers::yesOrNo("Use async implementation?") == true)
        {
            ximu3::NetworkDiscovery discovery(callback);
            helpers::wait(-1);
        }
        else
        {
            const auto devices = ximu3::NetworkDiscovery::scan(2000);
            std::cout << "Discovered " << devices.size() << " devices" << std::endl;
            printDevices(devices);
        }
    }

private:
    std::function<void(const std::vector<ximu3::XIMU3_DiscoveredNetworkDevice>&)> callback = [](const auto& devices)
    {
        std::cout << "Devices updated (" << devices.size() << " devices available)" << std::endl;
        printDevices(devices);
    };

    static void printDevices(const std::vector<ximu3::XIMU3_DiscoveredNetworkDevice>& devices)
    {
        for (const auto& device : devices)
        {
            std::cout << device.device_name << " - " <<
                      device.serial_number << ", RSSI: " <<
                      device.rssi << "%, Battery: " <<
                      device.battery << "%, " <<
                      XIMU3_charging_status_to_string(device.status) << ", " <<
                      XIMU3_tcp_connection_info_to_string(device.tcp_connection_info) << ", " <<
                      XIMU3_udp_connection_info_to_string(device.udp_connection_info) << std::endl;
            // std::cout << XIMU3_discovered_network_device_to_string(*device) << std::endl; // alternative to above
        }
    }
};
