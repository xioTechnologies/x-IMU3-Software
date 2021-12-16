#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>

void UdpConnection()
{
    if (YesOrNo("Search for connections?") == true)
    {
        printf("Searching for connections\n");
        const XIMU3_DiscoveredNetworkDevices devices = XIMU3_network_discovery_scan(2000);
        if (devices.length == 0)
        {
            printf("No UDP connections available\n");
            return;
        }
        printf("Found %s - %s\n", devices.array[0].device_name, devices.array[0].serial_number);
        const XIMU3_UdpConnectionInfo connectionInfo = devices.array[0].udp_connection_info;
        XIMU3_discovered_network_devices_free(devices);
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
