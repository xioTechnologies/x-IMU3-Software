#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

static void Callback(const XIMU3_CharArrays responses, void* context);

static void PrintResponses(const XIMU3_CharArrays responses);

void Commands()
{
    // Search for connection
    printf("Searching for connections\n");

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
    printf("Connection successful\n");

    // Define read/write setting commands
    const char* const commands[] = {
            "{\"deviceName\":null}", /* change null to a value to write setting */
            "{\"serialNumber\":null}",
            "{\"firmwareVersion\":null}",
            "{\"bootloaderVersion\":null}",
            "{\"hardwareVersion\":null}",
            "{\"invalidSettingKey\":null}", /* this command is deliberately invalid to demonstrate a failed command */
    };
    const int numberOfCommands = sizeof(commands) / sizeof(commands[0]);

    // Send commands
    if (AskQuestion("Use async implementation?"))
    {
        XIMU3_connection_send_commands_async(connection, commands, numberOfCommands, 2, 500, Callback, NULL);
        Wait(3);
    }
    else
    {
        const XIMU3_CharArrays responses = XIMU3_connection_send_commands(connection, commands, numberOfCommands, 2, 500);
        PrintResponses(responses);
        XIMU3_char_arrays_free(responses);
    }

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void Callback(const XIMU3_CharArrays responses, void* context)
{
    PrintResponses(responses);
    XIMU3_char_arrays_free(responses);
}

static void PrintResponses(const XIMU3_CharArrays responses)
{
    printf("%u commands confirmed\n", responses.length);

    for (uint32_t index = 0; index < responses.length; index++)
    {
        printf("%s\n", responses.array[index]);
    }
}
