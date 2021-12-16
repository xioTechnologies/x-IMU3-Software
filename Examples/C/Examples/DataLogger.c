#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

#define MAX_NUMBER_OF_CONNECTION (8)

static void Callback(const XIMU3_Result result, void* context);

static void PrintResult(const XIMU3_Result result);

void DataLogger()
{
    // Open all USB connections
    const XIMU3_DiscoveredSerialDevices devices = XIMU3_serial_discovery_scan_filter(2000, XIMU3_ConnectionTypeUsb);
    XIMU3_Connection* connections[MAX_NUMBER_OF_CONNECTION];
    uint32_t numberOfConnections = 0;
    for (uint32_t index = 0; index < devices.length; index++)
    {
        if (index > MAX_NUMBER_OF_CONNECTION)
        {
            break;
        }
        printf("%s\n", XIMU3_discovered_serial_device_to_string(devices.array[index]));
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
    XIMU3_discovered_serial_devices_free(devices);

    // Log data
    const char* directory = "C:/";
    const char* name = "Data Logger Example";
    if (YesOrNo("Use async implementation?") == true)
    {
        XIMU3_DataLogger* const data_logger = XIMU3_data_logger_new(directory, name, connections, numberOfConnections, Callback, NULL);
        Wait(3);
        XIMU3_data_logger_free(data_logger);
    }
    else
    {
        XIMU3_data_logger_log(directory, name, connections, numberOfConnections, 3);
    }

    // Close all connections
    for (uint32_t index = 0; index < numberOfConnections; index++)
    {
        XIMU3_connection_close(connections[index]);
        XIMU3_connection_free(connections[index]);
    }
}

static void Callback(const XIMU3_Result result, void* context)
{
    PrintResult(result);
}

static void PrintResult(const XIMU3_Result result)
{
    switch (result)
    {
        case XIMU3_ResultOk:
            printf("Ok\n");
            break;
        case XIMU3_ResultError:
            printf("Error\n");
            break;
    }
    // printf("%s\n", XIMU3_result_to_string(result)); // alternative to above
}
