#include "../Helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void Callback(const XIMU3_SerialAccessoryMessage message, void* context);

void SerialAccessoryLoopback()
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

    // Loopback
    XIMU3_connection_add_serial_accessory_callback(connection, Callback, NULL);

    uint8_t bytes[32];

    for (int value = 0; value < 256; value += sizeof(bytes))
    {
        
    }


    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void Callback(const XIMU3_SerialAccessoryMessage message, void* context)
{

}
