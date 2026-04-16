#include "../helpers.h"
#include "connection.h"
#include <stdio.h>
#include "Ximu3.h"

void mux_connection() {
    // Search for connection
    const XIMU3_Devices usb_devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeUsb);

    sleep(1); // wait for OS to release port

    if (usb_devices.length == 0) {
        printf("No USB connections available\n");
        return;
    }

    printf("Found %s\n", XIMU3_device_to_string(usb_devices.array[0]));

    // Open connection
    XIMU3_Connection *const usb_connection = XIMU3_connection_new_usb(usb_devices.array[0].usb_connection_config);

    XIMU3_devices_free(usb_devices);

    const XIMU3_Result result = XIMU3_connection_open(usb_connection);

    if (result != XIMU3_ResultOk) {
        printf("Unable to open %s. %s.\n", XIMU3_connection_get_config_string(usb_connection), XIMU3_result_to_string(result));
        XIMU3_connection_free(usb_connection);
        return;
    }

    // Mux connection
    if (yes_or_no("Connect automatically?")) {
        const XIMU3_Devices mux_devices = XIMU3_mux_scanner_scan(usb_connection, XIMU3_MAX_NUMBER_OF_MUX_CHANNELS, XIMU3_DEFAULT_RETRIES, XIMU3_DEFAULT_TIMEOUT);

        if (mux_devices.length == 0) {
            printf("No mux connections available\n");
            XIMU3_devices_free(mux_devices);
            goto cleanup;
        }

        printf("Found %s\n", XIMU3_device_to_string(mux_devices.array[0]));

        XIMU3_MuxConnectionConfig *config = mux_devices.array[0].mux_connection_config;

        XIMU3_Connection *const mux_connection = XIMU3_connection_new_mux(config);

        XIMU3_devices_free(mux_devices);

        run(mux_connection);

        XIMU3_connection_free(mux_connection);
    } else {
        XIMU3_MuxConnectionConfig *const config = XIMU3_mux_connection_config_new(0x41, usb_connection); // replace with actual connection config

        XIMU3_Connection *const mux_connection = XIMU3_connection_new_mux(config);

        XIMU3_mux_connection_config_free(config);

        run(mux_connection);

        XIMU3_connection_free(mux_connection);
    }

    // Close connection
cleanup:
    XIMU3_connection_close(usb_connection);
    XIMU3_connection_free(usb_connection);
}
