#pragma once

#include "../Helpers.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include "Ximu3.hpp"

class NetworkAnnouncement
{
public:
    NetworkAnnouncement()
    {
        ximu3::NetworkAnnouncement networkAnnouncement;

        if (networkAnnouncement.getResult() != ximu3::XIMU3_ResultOk)
        {
            std::cout << "Unable to open network announcement socket" << std::endl;
            return;
        }

        if (helpers::yesOrNo("Use async implementation?"))
        {
            networkAnnouncement.addCallback(callback);
            std::this_thread::sleep_for(std::chrono::seconds(60));
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
        std::cout << message.device_name << ", " <<
            message.serial_number << ", " <<
            message.ip_address << ", " <<
            message.tcp_port << ", " <<
            message.udp_send << ", " <<
            message.udp_receive << ", " <<
            message.rssi << "%, " <<
            message.battery << "%, " <<
            ximu3::XIMU3_charging_status_to_string(message.charging_status) << std::endl;
        // std::cout << ximu3::XIMU3_network_announcement_message_to_string(message) << std::endl; // alternative to above
    }
};
