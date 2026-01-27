#include "../helpers.h"
#include "connection.h"
#include <stdio.h>
#include "Ximu3.h"

void serial_connection() {
    if (yes_or_no("Search for connections?")) {
        const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeSerial);

        sleep(1); // wait for OS to release port

        if (devices.length == 0) {
            printf("No Serial connections available\n");
            return;
        }

        printf("Found %s\n", XIMU3_device_to_string(devices.array[0]));

        const XIMU3_SerialConnectionConfig config = devices.array[0].serial_connection_config;

        XIMU3_devices_free(devices);

        XIMU3_Connection *const connection = XIMU3_connection_new_serial(config);

        run(connection);
    } else {
        const XIMU3_SerialConnectionConfig config = (XIMU3_SerialConnectionConfig){
            .port_name = "COM1",
            .baud_rate = 115200,
            .rts_cts_enabled = false,
        }; // replace with actual connection config

        XIMU3_Connection *const connection = XIMU3_connection_new_serial(config);

        run(connection);
    }
}
