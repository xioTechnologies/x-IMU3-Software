#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_ConnectionStatus status, void *context);

static void print_connection_status(const XIMU3_ConnectionStatus status);

void keep_open() {
    // Search for connection
    const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeUsb);

    if (devices.length == 0) {
        printf("No USB connections available\n");
        return;
    }

    printf("Found %s\n", XIMU3_device_to_string(devices.array[0]));

    // Open connection
    XIMU3_Connection *const connection = XIMU3_connection_new_usb(devices.array[0].usb_connection_config);

    XIMU3_devices_free(devices);

    XIMU3_KeepOpen *const keep_open = XIMU3_keep_open_new(connection, callback, NULL);

    // Close connection
    sleep(60);

    XIMU3_keep_open_free(keep_open);
    XIMU3_connection_free(connection);
}

static void callback(const XIMU3_ConnectionStatus status, void *context) {
    print_connection_status(status);
}

static void print_connection_status(const XIMU3_ConnectionStatus status) {
    switch (status) {
        case XIMU3_ConnectionStatusConnected:
            printf("Connected\n");
            break;
        case XIMU3_ConnectionStatusReconnecting:
            printf("Reconnecting\n");
            break;
    }
    // printf("%s\n", XIMU3_connection_status_to_string(status)); // alternative to above
}
