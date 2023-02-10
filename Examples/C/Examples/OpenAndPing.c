#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>
#include <string.h>

static void PrintPingResponse(const XIMU3_PingResponse pingResponse);

static void Callback(const XIMU3_Result result, void* context);

void OpenAndPing()
{
    // Create connection info
    const XIMU3_UsbConnectionInfo connectionInfo = (XIMU3_UsbConnectionInfo) {
            .port_name = "COM1",
    };

    // Open and ping
    XIMU3_Connection* const connection = XIMU3_connection_new_usb(connectionInfo);
    if (YesOrNo("Use async implementation?") == true)
    {
        XIMU3_connection_open_async(connection, Callback, connection);
        Wait(3);
    }
    else
    {
        if (XIMU3_connection_open(connection) != XIMU3_ResultOk)
        {
            printf("Unable to open connection\n");
            XIMU3_connection_free(connection);
            return;
        }
        PrintPingResponse(XIMU3_connection_ping(connection));
    }

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void PrintPingResponse(const XIMU3_PingResponse pingResponse)
{
    if (strlen(pingResponse.interface) == 0)
    {
        printf("Ping failed\n");
        return;
    }
    printf("%s, %s, %s\n", pingResponse.interface, pingResponse.device_name, pingResponse.serial_number);
    // printf("%s\n", XIMU3_ping_response_to_string(pingResponse)); // alternative to above
}

static void Callback(const XIMU3_Result result, void* context)
{
    if (result != XIMU3_ResultOk)
    {
        printf("Unable to open connection\n");
        return;
    }
    PrintPingResponse(XIMU3_connection_ping(((XIMU3_Connection*) context)));
}
