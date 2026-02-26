#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_Devices devices, void *context);

static void print_devices(const XIMU3_Devices devices);

void mux_scanner() {
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

    // Mux scanner (blocking)
    const XIMU3_Devices mux_devices = XIMU3_mux_scanner_scan(usb_connection, XIMU3_MAX_NUMBER_OF_MUX_CHANNELS, XIMU3_DEFAULT_RETRIES, XIMU3_DEFAULT_TIMEOUT);

    print_devices(mux_devices);

    XIMU3_devices_free(mux_devices);

    // Mux scanner (non-blocking)
    XIMU3_MuxScanner *mux_scanner = XIMU3_mux_scanner_new(usb_connection, callback, NULL);

    sleep(60);

    XIMU3_mux_scanner_free(mux_scanner);

    // Close connection
    XIMU3_connection_close(usb_connection);
    XIMU3_connection_free(usb_connection);
}

static void callback(const XIMU3_Devices devices, void *context) {
    print_devices(devices);

    XIMU3_devices_free(devices);
}

static void print_devices(const XIMU3_Devices devices) {
    printf("%u device(s) found\n", devices.length);

    for (uint32_t index = 0; index < devices.length; index++) {
        const XIMU3_Device *const device = &devices.array[index];
        const char *connection_config;

        switch (device->connection_type) {
            case XIMU3_ConnectionTypeUsb:
                connection_config = XIMU3_usb_connection_config_to_string(device->usb_connection_config);
                break;
            case XIMU3_ConnectionTypeSerial:
                connection_config = XIMU3_serial_connection_config_to_string(device->serial_connection_config);
                break;
            case XIMU3_ConnectionTypeBluetooth:
                connection_config = XIMU3_bluetooth_connection_config_to_string(device->bluetooth_connection_config);
                break;
            default:
                connection_config = "";
                break;
        }

        printf("%s, %s, %s\n", device->device_name, device->serial_number, connection_config);
        // printf("%s\n", XIMU3_device_to_string(*device)); // alternative to above
    }
}
