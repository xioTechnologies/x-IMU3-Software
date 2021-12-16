#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>

void TcpConnection()
{
    if (YesOrNo("Search for connections?") == true)
    {
        printf("Searching for connections\n");
        const XIMU3_DiscoveredNetworkDevices devices = XIMU3_network_discovery_scan(2000);
        if (devices.length == 0)
        {
            printf("No TCP connections available\n");
            return;
        }
        printf("Found %s - %s\n", devices.array[0].device_name, devices.array[0].serial_number);
        const XIMU3_TcpConnectionInfo connectionInfo = devices.array[0].tcp_connection_info;
        XIMU3_discovered_network_devices_free(devices);
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
