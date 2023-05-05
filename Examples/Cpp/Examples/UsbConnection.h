#include "Connection.h"

class UsbConnection : public Connection
{
public:
    UsbConnection()
    {
        if (helpers::askQuestion("Search for connections?"))
        {
            std::cout << "Searching for connections" << std::endl;
            const auto devices = ximu3::PortScanner::scanFilter(ximu3::XIMU3_ConnectionTypeUsb);
            if (devices.size() == 0)
            {
                std::cout << "No USB connections available" << std::endl;
                return;
            }
            std::cout << "Found " << devices[0].device_name << " - " << devices[0].serial_number << std::endl;
            run(ximu3::UsbConnectionInfo(devices[0].usb_connection_info));
        }
        else
        {
            run(ximu3::UsbConnectionInfo("COM1"));
        }
    }
};
