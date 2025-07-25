#include "../Helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void Callback(const XIMU3_PingResponse pingResponse, void* context);

static void PrintPingResponse(const XIMU3_PingResponse pingResponse);

void Ping()
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

    const XIMU3_Result result = XIMU3_connection_open(connection);

    if (result != XIMU3_ResultOk)
    {
        printf("Unable to open connection. %s.\n", XIMU3_result_to_string(result));
        XIMU3_connection_free(connection);
        return;
    }

    // Ping
    if (YesOrNo("Use async implementation?"))
    {
        XIMU3_connection_ping_async(connection, Callback, NULL);

        Wait(3);
    }
    else
    {
        PrintPingResponse(XIMU3_connection_ping(connection));
    }

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void Callback(const XIMU3_PingResponse pingResponse, void* context)
{
    PrintPingResponse(pingResponse);
}

static void PrintPingResponse(const XIMU3_PingResponse pingResponse)
{
    if (pingResponse.result == XIMU3_ResultOk)
    {
        printf("%s, %s, %s\n", pingResponse.interface, pingResponse.device_name, pingResponse.serial_number);
        // printf("%s\n", XIMU3_ping_response_to_string(pingResponse)); // alternative to above
    }
    else
    {
        printf("Ping failed. %s.\n", XIMU3_result_to_string(pingResponse.result));
    }
}
