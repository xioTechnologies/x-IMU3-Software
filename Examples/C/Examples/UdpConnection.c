#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>

void UdpConnection()
{
    if (AskQuestion("Search for connections?"))
    {
        printf("Searching for connections\n");

        XIMU3_NetworkAnnouncement* const networkAnnouncement = XIMU3_network_announcement_new();

        if (XIMU3_network_announcement_get_result(networkAnnouncement) != XIMU3_ResultOk)
        {
            printf("Unable to open network announcement socket\n");
            XIMU3_network_announcement_free(networkAnnouncement);
            return;
        }

        const XIMU3_NetworkAnnouncementMessages messages = XIMU3_network_announcement_get_messages_after_short_delay(networkAnnouncement);

        XIMU3_network_announcement_free(networkAnnouncement);

        if (messages.length == 0)
        {
            printf("No UDP connections available\n");
            return;
        }
        printf("Found %s - %s\n", messages.array[0].device_name, messages.array[0].serial_number);

        const XIMU3_UdpConnectionInfo connectionInfo = XIMU3_network_announcement_message_to_udp_connection_info(messages.array[0]);

        XIMU3_network_announcement_messages_free(messages);

        Run(XIMU3_connection_new_udp(connectionInfo), XIMU3_udp_connection_info_to_string(connectionInfo));
    }
    else
    {
        const XIMU3_UdpConnectionInfo connectionInfo = (XIMU3_UdpConnectionInfo) {
                .ip_address = "192.168.1.1",
                .send_port = 9000,
                .receive_port = 8000,
        };
        Run(XIMU3_connection_new_udp(connectionInfo), XIMU3_udp_connection_info_to_string(connectionInfo));
    }
}
