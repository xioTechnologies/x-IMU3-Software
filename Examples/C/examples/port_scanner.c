#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

static void callback(const XIMU3_Devices devices, void *context);

static void print_devices(const XIMU3_Devices devices);

void port_scanner() {
    if (yes_or_no("Use async implementation?")) {
        XIMU3_PortScanner *const port_scanner = XIMU3_port_scanner_new(callback, NULL);

        sleep(60);

        XIMU3_port_scanner_free(port_scanner);
    } else {
        const XIMU3_Devices devices = XIMU3_port_scanner_scan();

        printf("Found %u devices\n", devices.length);

        print_devices(devices);

        XIMU3_devices_free(devices);
    }
}

static void callback(const XIMU3_Devices devices, void *context) {
    printf("Devices updated (%u devices available)\n", devices.length);

    print_devices(devices);

    XIMU3_devices_free(devices);
}

static void print_devices(const XIMU3_Devices devices) {
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
