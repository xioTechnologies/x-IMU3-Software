#include "../helpers.h"
#include <stdint.h>
#include <stdio.h>
#include "Ximu3.h"

static void print_data(const char *const direction, const uint8_t *const data, const size_t number_of_bytes);

void serial_accessory() {
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

    // Send accessory data
    const uint8_t data[] = "Any value \x00 to \xFF\n";
    const size_t data_length = sizeof(data) - 1; // exclude string termination

    const char *const data_as_json = XIMU3_bytes_to_json(data, data_length);

    char command[256];
    snprintf(command, sizeof(command), "{\"accessory\":%s}", data_as_json);

    const XIMU3_CommandMessage response = XIMU3_connection_send_command(connection, command, XIMU3_DEFAULT_RETRIES, XIMU3_DEFAULT_TIMEOUT);

    if (strlen(response.json) == 0) {
        printf("No response\n");
    } else if (strlen(response.error) > 0) {
        printf("%s\n", response.error);
    } else {
        print_data("TX", data, data_length);
    }

    // Receive accessory data
    sleep(1);

    const XIMU3_SerialAccessoryMessage message = XIMU3_connection_get_serial_accessory_message(connection, false);

    if (message.timestamp != 0) {
        print_data("RX", (uint8_t *) message.char_array, message.number_of_bytes);
    }

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}

static void print_data(const char *const direction, const uint8_t *const data, const size_t number_of_bytes) {
    printf("%s ", direction);

    for (size_t index = 0; index < number_of_bytes; index++) {
        printf("%02X ", data[index]);
    }

    printf("\n");
}
