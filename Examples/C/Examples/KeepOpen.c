#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

static void Callback(const XIMU3_ConnectionStatus status, void* context);

static void PrintConnectionStatus(const XIMU3_ConnectionStatus status);

void KeepOpen()
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

    XIMU3_KeepOpen* const keepOpen = XIMU3_keep_open_new(connection, Callback, NULL);

    // Close connection
    Wait(60);
    XIMU3_keep_open_free(keepOpen);
    XIMU3_connection_free(connection);
}

static void Callback(const XIMU3_ConnectionStatus status, void* context)
{
    PrintConnectionStatus(status);
}

static void PrintConnectionStatus(const XIMU3_ConnectionStatus status)
{
    switch (status)
    {
        case XIMU3_ConnectionStatusConnected:
            printf("Connected\n");
            break;
        case XIMU3_ConnectionStatusReconnecting:
            printf("Reconnecting\n");
            break;
    }
    // printf("%s\n", XIMU3_connection_status_to_string(status)); // alternative to above
}
