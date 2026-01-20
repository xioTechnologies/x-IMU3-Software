#include "../../../x-IMU3-API/C/Ximu3.h"
#include "connection.h"
#include <stdio.h>

void mux_connection() {
    // Search for connection
    const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeUsb);

    if (devices.length == 0) {
        printf("No USB connections available\n");
        return;
    }

    printf("Found %s\n", XIMU3_device_to_string(devices.array[0]));

    // Open connection
    XIMU3_Connection *const usb_connection = XIMU3_connection_new_usb(devices.array[0].usb_connection_config);

    XIMU3_devices_free(devices);

    const XIMU3_Result result = XIMU3_connection_open(usb_connection);

    if (result != XIMU3_ResultOk) {
        printf("Unable to open %s. %s.\n", XIMU3_connection_get_config_string(usb_connection), XIMU3_result_to_string(result));
        XIMU3_connection_free(usb_connection);
        return;
    }

    // Mux connection
    XIMU3_MuxConnectionConfig *const config = XIMU3_mux_connection_config_new(0x41, usb_connection);

    XIMU3_Connection *const mux_connection = XIMU3_connection_new_mux(config);

    run(mux_connection);

    XIMU3_mux_connection_config_free(config);

    // Close connection
    XIMU3_connection_close(usb_connection);
    XIMU3_connection_free(usb_connection);
}
