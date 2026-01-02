#include "../helpers.h"
#include <stdio.h>
#include <string.h>
#include "Ximu3.h"

static void callback(const XIMU3_CommandMessages responses, void* context);

static void print_responses(const XIMU3_CommandMessages responses);

void commands()
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

    // Example commands
    const char* const commands[] = {
        "{\"device_name\":\"Foobar\"}", // write "Foobar" to device name
        "{\"serial_number\":null}", // read serial number
        "{\"firmware_version\":null}", // read firmware version
        "{\"invalid_key\":null}", // invalid key to demonstrate an error response
    };
    const int number_of_commands = sizeof(commands) / sizeof(commands[0]);

    // Send commands
    if (yes_or_no("Use async implementation?"))
    {
        XIMU3_connection_send_commands_async(connection, commands, number_of_commands, XIMU3_DEFAULT_RETRIES, XIMU3_DEFAULT_TIMEOUT, callback, NULL);

        Wait(3);
    }
    else
    {
        const XIMU3_CommandMessages responses = XIMU3_connection_send_commands(connection, commands, number_of_commands, XIMU3_DEFAULT_RETRIES, XIMU3_DEFAULT_TIMEOUT);

        print_responses(responses);

        XIMU3_command_messages_free(responses);
    }

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void callback(const XIMU3_CommandMessages responses, void* context)
{
    print_responses(responses);

    XIMU3_command_messages_free(responses);
}

static void print_responses(const XIMU3_CommandMessages responses)
{
    for (uint32_t index = 0; index < responses.length; index++)
    {
        const XIMU3_CommandMessage* const response = &responses.array[index];

        if (strlen(response->json) == 0)
        {
            printf("No response");
            continue;
        }

        if (strlen(response->error) > 0)
        {
            printf("%s\n", response->error);
            continue;
        }

        printf("%s : %s\n", response->key, response->value);
    }
}
