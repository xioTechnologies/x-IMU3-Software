#include "../helpers.h"
#include "connection.h"
#include <stdio.h>
#include "Ximu3.h"

void bluetooth_connection()
{
    if (yes_or_no("Search for connections?"))
    {
        const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeBluetooth);

        if (devices.length == 0)
        {
            printf("No Bluetooth connections available\n");
            return;
        }
        printf("Found %s %s\n", devices.array[0].device_name, devices.array[0].serial_number);

        const XIMU3_BluetoothConnectionInfo connection_info = devices.array[0].bluetooth_connection_info;

        XIMU3_devices_free(devices);

        XIMU3_Connection* const connection = XIMU3_connection_new_bluetooth(connection_info);

        run(connection, XIMU3_bluetooth_connection_info_to_string(connection_info));
    }
    else
    {
        const XIMU3_BluetoothConnectionInfo connection_info = (XIMU3_BluetoothConnectionInfo) {
            .port_name = "COM1",
        }; // replace with actual connection info

        XIMU3_Connection* const connection = XIMU3_connection_new_bluetooth(connection_info);

        run(connection, XIMU3_bluetooth_connection_info_to_string(connection_info));
    }
}
