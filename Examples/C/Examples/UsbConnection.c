#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>

void UsbConnection()
{
    if (YesOrNo("Search for connections?") == true)
    {
        printf("Searching for connections\n");
        const XIMU3_DiscoveredSerialDevices devices = XIMU3_serial_discovery_scan_filter(2000, XIMU3_ConnectionTypeUsb);
        if (devices.length == 0)
        {
            printf("No USB connections available\n");
            return;
        }
        printf("Found %s - %s\n", devices.array[0].device_name, devices.array[0].serial_number);
        const XIMU3_UsbConnectionInfo connectionInfo = devices.array[0].usb_connection_info;
        XIMU3_discovered_serial_devices_free(devices);
        Run(XIMU3_connection_new_usb(connectionInfo), XIMU3_usb_connection_info_to_string(connectionInfo));
    }
    else
    {
        const XIMU3_UsbConnectionInfo connectionInfo = (XIMU3_UsbConnectionInfo) {
                .port_name = "COM1",
        };
        Run(XIMU3_connection_new_usb(connectionInfo), XIMU3_usb_connection_info_to_string(connectionInfo));
    }
}
