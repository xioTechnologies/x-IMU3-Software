#pragma once

#include "Connection.h"

class MuxConnection : public Connection
{
public:
    MuxConnection()
    {
        // Search for connection
        const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_PortTypeUsb);

        if (devices.empty())
        {
            std::cout << "No USB connections available" << std::endl;
            return;
        }

        std::cout << "Found " << devices[0].device_name << " " << devices[0].serial_number << std::endl;

        // Open connection
        ximu3::Connection usbConnection(*ximu3::ConnectionInfo::from(devices[0]));

        if (usbConnection.open() != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Unable to open connection" << std::endl;
            return;
        }

        ximu3::MuxConnectionInfo connectionInfo(0x41, usbConnection);

        run(connectionInfo);
    }
};
