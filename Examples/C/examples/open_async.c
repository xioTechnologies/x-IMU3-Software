#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_Result result, void *context);

void open_async() {
    // Search for connection
    const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeUsb);

    sleep(1); // wait for OS to release port

    if (devices.length == 0) {
        printf("No USB connections available\n");
        return;
    }

    printf("Found %s\n", XIMU3_device_to_string(devices.array[0]));

    // Open connection
    XIMU3_Connection *const connection = XIMU3_connection_new_usb(devices.array[0].usb_connection_config);

    XIMU3_devices_free(devices);

    XIMU3_connection_open_async(connection, callback, connection);

    // Close connection
    sleep(3);

    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void callback(const XIMU3_Result result, void *context) {
    if (result != XIMU3_ResultOk) {
        XIMU3_Connection *const connection = (XIMU3_Connection *) context;

        printf("Unable to open %s. %s.\n", XIMU3_connection_get_config_string(connection), XIMU3_result_to_string(result));
        return;
    }

    printf("%s\n", XIMU3_result_to_string(result));
}
