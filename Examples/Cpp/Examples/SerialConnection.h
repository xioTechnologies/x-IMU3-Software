#pragma once

#include "Connection.h"

class SerialConnection : public Connection
{
public:
    SerialConnection()
    {
        if (helpers::yesOrNo("Search for connections?"))
        {
            const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_ConnectionTypeSerial);

            if (devices.empty())
            {
                std::cout << "No serial connections available" << std::endl;
                return;
            }

            std::cout << "Found " << devices[0].device_name << " " << devices[0].serial_number << std::endl;

            const auto connectionInfo = ximu3::connectionInfoFrom(devices[0]);

            run(*connectionInfo);
        }
        else
        {
            const ximu3::SerialConnectionInfo connectionInfo("COM1", 115200, false); // replace with actual connection info

            run(connectionInfo);
        }
    }
};
