#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_Result result, void* context);

void open_async()
{
    // Search for connection
    const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeUsb);

    if (devices.length == 0)
    {
        printf("No USB connections available\n");
        return;
    }
    printf("Found %s %s\n", devices.array[0].device_name, devices.array[0].serial_number);

    // Open connection
    XIMU3_Connection* const connection = XIMU3_connection_new_usb(devices.array[0].usb_connection_info);

    XIMU3_devices_free(devices);

    XIMU3_connection_open_async(connection, callback, NULL);

    // Close connection
    Wait(3);
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void callback(const XIMU3_Result result, void* context)
{
    printf("%s\n", XIMU3_result_to_string(result));
}
