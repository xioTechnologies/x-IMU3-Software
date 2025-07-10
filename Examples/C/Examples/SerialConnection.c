#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>
#include "Ximu3.h"

void SerialConnection()
{
    if (YesOrNo("Search for connections?"))
    {
        const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeSerial);

        if (devices.length == 0)
        {
            printf("No Serial connections available\n");
            return;
        }
        printf("Found %s %s\n", devices.array[0].device_name, devices.array[0].serial_number);

        const XIMU3_SerialConnectionInfo connectionInfo = devices.array[0].serial_connection_info;

        XIMU3_devices_free(devices);

        XIMU3_Connection* const connection = XIMU3_connection_new_serial(connectionInfo);

        Run(connection, XIMU3_serial_connection_info_to_string(connectionInfo));
    }
    else
    {
        const XIMU3_SerialConnectionInfo connectionInfo = (XIMU3_SerialConnectionInfo) {
            .port_name = "COM1",
            .baud_rate = 115200,
            .rts_cts_enabled = false,
        }; // replace with actual connection info

        XIMU3_Connection* const connection = XIMU3_connection_new_serial(connectionInfo);

        Run(connection, XIMU3_serial_connection_info_to_string(connectionInfo));
    }
}
