#include "../helpers.h"
#include "connection.h"
#include <stdio.h>
#include "Ximu3.h"

void udp_connection()
{
    if (yes_or_no("Search for connections?"))
    {
        XIMU3_NetworkAnnouncement* const network_announcement = XIMU3_network_announcement_new();

        const XIMU3_NetworkAnnouncementMessages messages = XIMU3_network_announcement_get_messages_after_short_delay(network_announcement);

        XIMU3_network_announcement_free(network_announcement);

        if (messages.length == 0)
        {
            printf("No UDP connections available\n");
            return;
        }
        printf("Found %s %s\n", messages.array[0].device_name, messages.array[0].serial_number);

        const XIMU3_UdpConnectionInfo connection_info = XIMU3_network_announcement_message_to_udp_connection_info(messages.array[0]);

        XIMU3_network_announcement_messages_free(messages);

        XIMU3_Connection* const connection = XIMU3_connection_new_udp(connection_info);

        run(connection, XIMU3_udp_connection_info_to_string(connection_info));
    }
    else
    {
        const XIMU3_UdpConnectionInfo connection_info = (XIMU3_UdpConnectionInfo) {
            .ip_address = "192.168.1.1",
            .send_port = 9000,
            .receive_port = 8000,
        }; // replace with actual connection info

        XIMU3_Connection* const connection = XIMU3_connection_new_udp(connection_info);

        run(connection, XIMU3_udp_connection_info_to_string(connection_info));
    }
}
