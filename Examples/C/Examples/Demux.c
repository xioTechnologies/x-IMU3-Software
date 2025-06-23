#include "../Helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void PrintResult(const XIMU3_Result result);

void Demux()
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

    if (XIMU3_connection_open(connection) != XIMU3_ResultOk)
    {
        printf("Unable to open connection\n");
        XIMU3_connection_free(connection);
        return;
    }

    // Demux
    const uint8_t channels[] = { 0x41, 0x42, 0x43 };
    const int numberOfChannels = sizeof(channels) / sizeof(channels[0]);

    XIMU3_Demux* const demux = XIMU3_demux_new(connection, channels, numberOfChannels);

    const XIMU3_Result result = XIMU3_demux_get_result(demux);

    PrintResult(result);

    if (result == XIMU3_ResultOk)
    {
        Wait(60);
    }

    XIMU3_demux_free(demux);

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void PrintResult(const XIMU3_Result result)
{
    switch (result)
    {
        case XIMU3_ResultOk:
            printf("OK\n");
            break;
        case XIMU3_ResultError:
            printf("Error\n");
            break;
    }
    // printf("%s\n", XIMU3_result_to_string(result)); // alternative to above
}
