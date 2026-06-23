#include "../helpers.h"
#include <stdio.h>
#include "Ximu3.h"

void settings() {
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

    // Backup settings
    const char *file_path = "C:/Users/Public/x-IMU3 Example Settings.json";

    const XIMU3_Result resultBackup = XIMU3_settings_backup(file_path, connection);

    if (resultBackup != XIMU3_ResultOk) {
        printf("Backup failed. %s.\n", XIMU3_result_to_string(resultBackup));
    }

    // Restore settings
    const XIMU3_Result resultRestore = XIMU3_settings_restore(file_path, connection);

    if (resultRestore != XIMU3_ResultOk) {
        printf("Restore failed. %s.\n", XIMU3_result_to_string(resultRestore));
    }

    // Save command
    XIMU3_connection_send_command(connection, "{\"save\":null}", XIMU3_DEFAULT_RETRIES, XIMU3_DEFAULT_TIMEOUT);

    // Close connection
    XIMU3_connection_close(connection);
    XIMU3_connection_free(connection);
}
