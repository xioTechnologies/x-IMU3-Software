#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>

void TcpConnection()
{
    if (Question("Search for connections?"))
    {
        printf("Searching for connections\n");
        XIMU3_NetworkAnnouncement* const network_announcement = XIMU3_network_announcement_new();
        const XIMU3_NetworkAnnouncementMessages messages = XIMU3_network_announcement_get_messages_after_short_delay(network_announcement);
        XIMU3_network_announcement_free(network_announcement);
        if (messages.length == 0)
        {
            printf("No TCP connections available\n");
            return;
        }
        printf("Found %s - %s\n", messages.array[0].device_name, messages.array[0].serial_number);
        const XIMU3_TcpConnectionInfo connectionInfo = XIMU3_network_announcement_message_to_tcp_connection_info(messages.array[0]);
        XIMU3_network_announcement_messages_free(messages);
        Run(XIMU3_connection_new_tcp(connectionInfo), XIMU3_tcp_connection_info_to_string(connectionInfo));
    }
    else
    {
        const XIMU3_TcpConnectionInfo connectionInfo = (XIMU3_TcpConnectionInfo) {
                .ip_address = "192.168.1.1",
                .port = 7000,
        };
        Run(XIMU3_connection_new_tcp(connectionInfo), XIMU3_tcp_connection_info_to_string(connectionInfo));
    }
}
