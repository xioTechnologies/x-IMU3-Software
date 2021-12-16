#include "Connection.h"

class SerialConnection : public Connection
{
public:
    SerialConnection()
    {
        if (helpers::yesOrNo("Search for connections?") == true)
        {
            std::cout << "Searching for connections" << std::endl;
            const auto devices = ximu3::SerialDiscovery::scanFilter(2000, ximu3::XIMU3_ConnectionTypeSerial);
            if (devices.size() == 0)
            {
                std::cout << "No serial connections available" << std::endl;
                return;
            }
            std::cout << "Found " << devices[0].device_name << " - " << devices[0].serial_number << std::endl;
            run(ximu3::SerialConnectionInfo(devices[0].serial_connection_info));
        }
        else
        {
            run(ximu3::SerialConnectionInfo("COM1", 115200, false));
        }
    }
};
