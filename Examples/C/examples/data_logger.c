#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

#define MAX_NUMBER_OF_CONNECTION (8)

void data_logger() {
    // Open all USB connections
    const XIMU3_Devices devices = XIMU3_port_scanner_scan_filter(XIMU3_PortTypeUsb);

    XIMU3_Connection *connections[MAX_NUMBER_OF_CONNECTION];
    uint32_t number_of_connections = 0;

    for (uint32_t index = 0; index < devices.length; index++) {
        if (index > MAX_NUMBER_OF_CONNECTION) {
            break;
        }

        printf("Found %s\n", XIMU3_device_to_string(devices.array[index]));

        XIMU3_Connection *const connection = XIMU3_connection_new_usb(devices.array[index].usb_connection_config);

        const XIMU3_Result result = XIMU3_connection_open(connection);

        if (result == XIMU3_ResultOk) {
            connections[number_of_connections++] = connection;
        } else {
            printf("Unable to open %s. %s.\n", XIMU3_connection_get_config_string(connection), XIMU3_result_to_string(result));
            XIMU3_connection_free(connection);
        }
    }
    XIMU3_devices_free(devices);

    if (number_of_connections == 0) {
        printf("No USB connections available\n");
        return;
    }

    // Log data
    const char *destination = "C:/";
    const char *name = "x-IMU3 Data Logger Example";

    if (yes_or_no("Use async implementation?")) {
        XIMU3_DataLogger *const data_logger = XIMU3_data_logger_new(destination, name, connections, number_of_connections);

        const XIMU3_Result result = XIMU3_data_logger_get_result(data_logger);

        if (result != XIMU3_ResultOk) {
            printf("Data logger failed. %s.\n", XIMU3_result_to_string(result));
        }

        sleep(3);

        XIMU3_data_logger_free(data_logger);
    } else {
        const XIMU3_Result result = XIMU3_data_logger_log(destination, name, connections, number_of_connections, 3);

        if (result != XIMU3_ResultOk) {
            printf("Data logger failed. %s.\n", XIMU3_result_to_string(result));
        }
    }

    printf("Complete\n");

    // Close all connections
    for (uint32_t index = 0; index < number_of_connections; index++) {
        XIMU3_connection_close(connections[index]);
        XIMU3_connection_free(connections[index]);
    }
}
