#include "../../../x-IMU3-API/C/Ximu3.h"
#include <stdio.h>

void Ping()
{
    // Search for connection
    const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_ConnectionTypeUsb);

    if (devices.length == 0)
    {
        printf("No USB connections available\n");
        return;
    }
    printf("Found %s %s\n", devices.array[0].device_name, devices.array[0].serial_number);

    // Open connection
    XIMU3_Connection* const connection = XIMU3_connection_new_usb(devices.array[0].usb_connection_info);

    XIMU3_devices_free(devices);

    if (XIMU3_connection_open(connection) != XIMU3_ResultOk)
    {
        printf("Unable to open connection\n");
        XIMU3_connection_free(connection);
        return;
    }

    // Ping
    const XIMU3_PingResponse response = XIMU3_connection_ping(connection);

    if (response.result == XIMU3_ResultOk)
    {
        printf("%s, %s, %s\n", response.interface, response.device_name, response.serial_number);
        // printf("%s\n", XIMU3_ping_response_to_string(response)); // alternative to above
    }
    else
    {
        printf("No response\n");
    }

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}
