#include "../helpers.h"
#include "connection.h"
#include <stdio.h>
#include "Ximu3.h"

void bluetooth_connection() {
    if (yes_or_no("Search for connections?")) {
        const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeBluetooth);

        if (devices.length == 0) {
            printf("No Bluetooth connections available\n");
            return;
        }

        printf("Found %s\n", XIMU3_device_to_string(devices.array[0]));

        const XIMU3_BluetoothConnectionConfig config = devices.array[0].bluetooth_connection_config;

        XIMU3_devices_free(devices);

        XIMU3_Connection *const connection = XIMU3_connection_new_bluetooth(config);

        run(connection);
    } else {
        const XIMU3_BluetoothConnectionConfig config = (XIMU3_BluetoothConnectionConfig){
            .port_name = "COM1",
        }; // replace with actual connection config

        XIMU3_Connection *const connection = XIMU3_connection_new_bluetooth(config);

        run(connection);
    }
}
