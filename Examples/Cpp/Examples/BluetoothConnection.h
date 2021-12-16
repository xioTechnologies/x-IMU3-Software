#include "Connection.h"

class BluetoothConnection : public Connection
{
public:
    BluetoothConnection()
    {
        if (helpers::yesOrNo("Search for connections?") == true)
        {
            std::cout << "Searching for connections" << std::endl;
            const auto devices = ximu3::SerialDiscovery::scanFilter(2000, ximu3::XIMU3_ConnectionTypeBluetooth);
            if (devices.size() == 0)
            {
                std::cout << "No Bluetooth connections available" << std::endl;
                return;
            }
            std::cout << "Found " << devices[0].device_name << " - " << devices[0].serial_number << std::endl;
            run(ximu3::BluetoothConnectionInfo(devices[0].bluetooth_connection_info));
        }
        else
        {
            run(ximu3::BluetoothConnectionInfo("COM1"));
        }
    }
};
