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
    printf("Found %s %s\n", devices.array[0].device_name, devices.array[0].serial_number);

    // Open connection
    XIMU3_Connection *const usb_connection = XIMU3_connection_new_usb(devices.array[0].usb_connection_info);

    XIMU3_devices_free(devices);

    if (XIMU3_connection_open(usb_connection) != XIMU3_ResultOk) {
        printf("Unable to open connection\n");
        XIMU3_connection_free(usb_connection);
        return;
    }

    // Mux connection
    XIMU3_MuxConnectionInfo *const mux_connection_info = XIMU3_mux_connection_info_new(0x41, usb_connection);

    XIMU3_Connection *const mux_connection = XIMU3_connection_new_mux(mux_connection_info);

    run(mux_connection, XIMU3_mux_connection_info_to_string(mux_connection_info));

    XIMU3_mux_connection_info_free(mux_connection_info);

    // Close connection
    XIMU3_connection_close(usb_connection);
    XIMU3_connection_free(usb_connection);
}
