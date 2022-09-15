#include "../../../x-IMU3-API/Cpp/Ximu3.hpp"
#include "../Helpers.hpp"
#include <iostream>

class NetworkAnnouncement
{
public:
    NetworkAnnouncement()
    {
        ximu3::NetworkAnnouncement networkAnnouncement;

        if (helpers::yesOrNo("Use async implementation?") == true)
        {
            networkAnnouncement.addCallback(callback);
            helpers::wait(-1);
        }
        else
        {
            for (const auto& message : networkAnnouncement.getMessagesAfterShortDelay())
            {
                printMessage(message);
            }
        }
    }

private:
    std::function<void(ximu3::XIMU3_NetworkAnnouncementMessage)> callback = [](const auto& message)
    {
        printMessage(message);
    };

    static void printMessage(const ximu3::XIMU3_NetworkAnnouncementMessage& message)
    {
        std::cout << message.device_name << " - " <<
                  message.serial_number << ", RSSI: " <<
                  message.rssi << "%, Battery: " <<
                  message.battery << "%, " <<
                  XIMU3_charging_status_to_string(message.status) << ", " <<
                  XIMU3_tcp_connection_info_to_string(message.tcp_connection_info) << ", " <<
                  XIMU3_udp_connection_info_to_string(message.udp_connection_info) << std::endl;
        // std::cout << XIMU3_network_announcement_message_to_string(message) << std::endl; // alternative to above
    }
};
