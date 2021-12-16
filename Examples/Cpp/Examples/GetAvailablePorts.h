#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <iostream>

class GetAvailablePorts
{
public:
    GetAvailablePorts()
    {
        const auto portNames = ximu3::SerialDiscovery::getAvailablePorts();
        if (portNames.size() == 0)
        {
            std::cout << "No ports available" << std::endl;
            return;
        }
        for (const auto& portName : portNames)
        {
            std::cout << portName << std::endl;
        }
    }
};
