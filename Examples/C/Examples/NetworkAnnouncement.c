#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <inttypes.h> // PRIu16
#include <stdio.h>

static void Callback(const XIMU3_NetworkAnnouncementMessage message, void* context);

static void PrintMessage(const XIMU3_NetworkAnnouncementMessage message);

void NetworkAnnouncement()
{
    XIMU3_NetworkAnnouncement* const networkAnnouncement = XIMU3_network_announcement_new();

    if (AskQuestion("Use async implementation?"))
    {
        XIMU3_network_announcement_add_callback(networkAnnouncement, Callback, NULL);
        Wait(60);
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
    printf("%s, %s, %s, %" PRIu16 ", %" PRIu16 ", %" PRIu16 ", %d%%, %d%%, %s\n",
           message.device_name,
           message.serial_number,
           message.ip_address,
           message.tcp_port,
           message.udp_send,
           message.udp_receive,
           message.rssi,
           message.battery,
           XIMU3_charging_status_to_string(message.charging_status));
    // printf("%s\n", XIMU3_network_announcement_message_to_string(message)); // alternative to above
}
