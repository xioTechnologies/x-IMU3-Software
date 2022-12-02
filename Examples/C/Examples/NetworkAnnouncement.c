#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

static void Callback(const XIMU3_NetworkAnnouncementMessage message, void* context);

static void PrintMessage(const XIMU3_NetworkAnnouncementMessage message);

void NetworkAnnouncement()
{
    XIMU3_NetworkAnnouncement* const networkAnnouncement = XIMU3_network_announcement_new();

    if (YesOrNo("Use async implementation?") == true)
    {
        XIMU3_network_announcement_add_callback(networkAnnouncement, Callback, NULL);
        Wait(-1);
    }
    else
    {
        const XIMU3_NetworkAnnouncementMessages messages = XIMU3_network_announcement_get_messages(networkAnnouncement);

        for (uint32_t index = 0; index < messages.length; index++)
        {
            const XIMU3_NetworkAnnouncementMessage* const message = &messages.array[index];
            PrintMessage(*message);
        }

        XIMU3_network_announcement_messages_free(messages);
    }

    XIMU3_network_announcement_free(networkAnnouncement);
}

static void Callback(const XIMU3_NetworkAnnouncementMessage message, void* context)
{
    PrintMessage(message);
}

static void PrintMessage(const XIMU3_NetworkAnnouncementMessage message)
{
    printf("%s - %s, RSSI: %d%%, Battery: %d%%, %s, %s, %s\n",
           message.device_name,
           message.serial_number,
           message.rssi,
           message.battery,
           XIMU3_charging_status_to_string(message.status),
           XIMU3_tcp_connection_info_to_string(message.tcp_connection_info),
           XIMU3_udp_connection_info_to_string(message.udp_connection_info));
    // printf("%s\n", XIMU3_network_announcement_message_to_string(message)); // alternative to above
}
