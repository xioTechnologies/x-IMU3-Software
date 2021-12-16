#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include "Connection.h"
#include <stdio.h>

void BluetoothConnection()
{
    if (YesOrNo("Search for connections?") == true)
    {
        printf("Searching for connections\n");
        const XIMU3_DiscoveredSerialDevices devices = XIMU3_serial_discovery_scan_filter(2000, XIMU3_ConnectionTypeBluetooth);
        if (devices.length == 0)
        {
            printf("No Bluetooth connections available\n");
            return;
        }
        printf("Found %s - %s\n", devices.array[0].device_name, devices.array[0].serial_number);
        const XIMU3_BluetoothConnectionInfo connectionInfo = devices.array[0].bluetooth_connection_info;
        XIMU3_discovered_serial_devices_free(devices);
        Run(XIMU3_connection_new_bluetooth(connectionInfo), XIMU3_bluetooth_connection_info_to_string(connectionInfo));
    }
    else
    {
        const XIMU3_BluetoothConnectionInfo connectionInfo = (XIMU3_BluetoothConnectionInfo) {
                .port_name = "COM1",
        };
        Run(XIMU3_connection_new_bluetooth(connectionInfo), XIMU3_bluetooth_connection_info_to_string(connectionInfo));
    }
}
