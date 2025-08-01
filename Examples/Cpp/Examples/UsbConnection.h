#pragma once

#include "Connection.h"

class UsbConnection : public Connection
{
public:
    UsbConnection()
    {
        if (helpers::yesOrNo("Search for connections?"))
        {
            const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

            if (devices.empty())
            {
                std::cout << "No USB connections available" << std::endl;
                return;
            }

            std::cout << "Found " << devices[0].device_name << " " << devices[0].serial_number << std::endl;

            const auto connectionInfo = ximu3::ConnectionInfo::from(devices[0]);

            run(*connectionInfo);
        }
        else
        {
            const ximu3::UsbConnectionInfo connectionInfo("COM1"); // replace with actual connection info

            run(connectionInfo);
        }
    }
};
