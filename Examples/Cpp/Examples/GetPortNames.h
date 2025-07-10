#pragma once

#include "../Helpers.hpp"
#include <iostream>
#include "Ximu3.hpp"

class GetPortNames
{
public:
    GetPortNames()
    {
        const auto portNames = ximu3::PortScanner::getPortNames();

        if (portNames.empty())
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
