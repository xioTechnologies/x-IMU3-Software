#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

#define MAX_NUMBER_OF_CONNECTION (8)

static void PrintResult(const XIMU3_Result result);

void DataLogger()
{
    // Open all USB connections
    const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeUsb);

    XIMU3_Connection* connections[MAX_NUMBER_OF_CONNECTION];
    uint32_t numberOfConnections = 0;

    for (uint32_t index = 0; index < devices.length; index++)
    {
        if (index > MAX_NUMBER_OF_CONNECTION)
        {
            break;
        }
        printf("%s\n", XIMU3_device_to_string(devices.array[index]));

        XIMU3_Connection* const connection = XIMU3_connection_new_usb(devices.array[index].usb_connection_info);

        if (XIMU3_connection_open(connection) == XIMU3_ResultOk)
        {
            connections[numberOfConnections++] = connection;
        }
        else
        {
            printf("Unable to open connection\n");
            XIMU3_connection_free(connection);
        }
    }
    XIMU3_devices_free(devices);

    if (numberOfConnections == 0)
    {
        printf("No USB connections available\n");
        return;
    }

    // Log data
    const char* destination = "C:/";
    const char* name = "Data Logger Example";

    if (YesOrNo("Use async implementation?"))
    {
        XIMU3_DataLogger* const dataLogger = XIMU3_data_logger_new(destination, name, connections, numberOfConnections);

        const XIMU3_Result result = XIMU3_data_logger_get_result(dataLogger);

        if (result == XIMU3_ResultOk)
        {
            Wait(3);
        }

        PrintResult(result);

        XIMU3_data_logger_free(dataLogger);
    }
    else
    {
        PrintResult(XIMU3_data_logger_log(destination, name, connections, numberOfConnections, 3));
    }

    // Close all connections
    for (uint32_t index = 0; index < numberOfConnections; index++)
    {
        XIMU3_connection_close(connections[index]);
        XIMU3_connection_free(connections[index]);
    }
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
