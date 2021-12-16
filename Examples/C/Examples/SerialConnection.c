#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>

void SerialConnection()
{
    if (YesOrNo("Search for connections?") == true)
    {
        printf("Searching for connections\n");
        const XIMU3_DiscoveredSerialDevices devices = XIMU3_serial_discovery_scan_filter(2000, XIMU3_ConnectionTypeSerial);
        if (devices.length == 0)
        {
            printf("No serial connections available\n");
            return;
        }
        printf("Found %s - %s\n", devices.array[0].device_name, devices.array[0].serial_number);
        const XIMU3_SerialConnectionInfo connectionInfo = devices.array[0].serial_connection_info;
        XIMU3_discovered_serial_devices_free(devices);
        Run(XIMU3_connection_new_serial(connectionInfo), XIMU3_serial_connection_info_to_string(connectionInfo));
    }
    else
    {
        const XIMU3_SerialConnectionInfo connectionInfo = (XIMU3_SerialConnectionInfo) {
                .port_name = "COM1",
                .baud_rate = 115200,
                .rts_cts_enabled = false,
        };
        Run(XIMU3_connection_new_serial(connectionInfo), XIMU3_serial_connection_info_to_string(connectionInfo));
    }
}
