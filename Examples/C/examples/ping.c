#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_PingResponse response, void *context);

static void print_response(const XIMU3_PingResponse response);

void ping() {
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

    const XIMU3_Result result = XIMU3_connection_open(connection);

    if (result != XIMU3_ResultOk) {
        printf("Unable to open %s. %s.\n", XIMU3_connection_get_config_string(connection), XIMU3_result_to_string(result));
        XIMU3_connection_free(connection);
        return;
    }

    // Ping
    if (yes_or_no("Use async implementation?")) {
        XIMU3_connection_ping_async(connection, callback, NULL);

        sleep(3);
    } else {
        print_response(XIMU3_connection_ping(connection));
    }

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void callback(const XIMU3_PingResponse response, void *context) {
    print_response(response);
}

static void print_response(const XIMU3_PingResponse response) {
    if (strlen(response.interface) == 0) {
        printf("No response\n");
        return;
    }

    printf("%s, %s, %s\n", response.interface, response.device_name, response.serial_number);
    // printf("%s\n", XIMU3_ping_response_to_string(response)); // alternative to above
}
