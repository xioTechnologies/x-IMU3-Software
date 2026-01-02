#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_PingResponse ping_response, void* context);

static void print_ping_response(const XIMU3_PingResponse ping_response);

void ping()
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
    if (yes_or_no("Use async implementation?"))
    {
        XIMU3_connection_ping_async(connection, callback, NULL);

        Wait(3);
    }
    else
    {
        print_ping_response(XIMU3_connection_ping(connection));
    }

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void callback(const XIMU3_PingResponse ping_response, void* context)
{
    print_ping_response(ping_response);
}

static void print_ping_response(const XIMU3_PingResponse ping_response)
{
    if (strlen(ping_response.interface) == 0)
    {
        printf("No response");
        return;
    }

    printf("%s, %s, %s\n", ping_response.interface, ping_response.device_name, ping_response.serial_number);
    // printf("%s\n", XIMU3_ping_response_to_string(ping_response)); // alternative to above
}
