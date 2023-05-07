#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>

void UsbConnection()
{
    if (AskQuestion("Search for connections?"))
    {
        printf("Searching for connections\n");

        const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_ConnectionTypeUsb);

        if (devices.length == 0)
        {
            printf("No USB connections available\n");
            return;
        }
        printf("Found %s - %s\n", devices.array[0].device_name, devices.array[0].serial_number);

        const XIMU3_UsbConnectionInfo connectionInfo = devices.array[0].usb_connection_info;

        XIMU3_devices_free(devices);

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
