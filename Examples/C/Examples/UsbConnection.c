#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>
#include "Ximu3.h"

void UsbConnection()
{
    if (YesOrNo("Search for connections?"))
    {
        const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeUsb);

        if (devices.length == 0)
        {
            printf("No USB connections available\n");
            return;
        }
        printf("Found %s %s\n", devices.array[0].device_name, devices.array[0].serial_number);

        const XIMU3_UsbConnectionInfo connectionInfo = devices.array[0].usb_connection_info;

        XIMU3_devices_free(devices);

        XIMU3_Connection* const connection = XIMU3_connection_new_usb(connectionInfo);

        Run(connection, XIMU3_usb_connection_info_to_string(connectionInfo));
    }
    else
    {
        const XIMU3_UsbConnectionInfo connectionInfo = (XIMU3_UsbConnectionInfo) {
            .port_name = "COM1",
        }; // replace with actual connection info

        XIMU3_Connection* const connection = XIMU3_connection_new_usb(connectionInfo);

        Run(connection, XIMU3_usb_connection_info_to_string(connectionInfo));
    }
}
