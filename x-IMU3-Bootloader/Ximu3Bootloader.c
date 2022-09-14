#include "ds30_loader.h"
#include "Ximu3Bootloader.h"

//#define DEBUG_MODE
#ifdef DEBUG_MODE

#include <inttypes.h>

static void callback(const void* const ds30_options, const ds30_event_t event, const bool boolean, const void* const voidPointer)
{
    switch (event)
    {
        case EV_OP_START:
            printf("EV_OP_START\n");
            break;
        case EV_OP_PARSE_HEX_FILE:
            printf("EV_OP_PARSE_HEX_FILE\n");
            break;
        case EV_OP_PORT_OPEN_RESET:
            printf("EV_OP_PORT_OPEN_RESET\n");
            break;
        case EV_OP_PORT_OPEN:
            printf("EV_OP_PORT_OPEN\n");
            break;
        case EV_OP_RESET_DEVICE:
            printf("EV_OP_RESET_DEVICE\n");
            break;
        case EV_OP_HELLO_RSP_RCV:
            printf("EV_OP_HELLO_RSP_RCV\n");
            break;
        case EV_OP_ESTABLISH_COMM:
            printf("EV_OP_ESTABLISH_COMM\n");
            break;
        case EV_OP_WRITE_FLASH:
            printf("EV_OP_WRITE_FLASH\n");
            break;
        case EV_OP_WRITE_AUX_FLASH:
            printf("EV_OP_WRITE_AUX_FLASH\n");
            break;
        case EV_OP_WRITE_EEPROM:
            printf("EV_OP_WRITE_EEPROM\n");
            break;
        case EV_OP_WRITE_CONFIGS:
            printf("EV_OP_WRITE_CONFIGS\n");
            break;
        case EV_OP_WRITE_PAGE:
            printf("EV_OP_WRITE_PAGE\n");
            break;
        case EV_OP_READ_FLASH:
            printf("EV_OP_READ_FLASH\n");
            break;
        case EV_OP_READ_PAGE:
            printf("EV_OP_READ_PAGE\n");
            break;
        case EV_OP_READ_EEPROM:
            printf("EV_OP_READ_EEPROM\n");
            break;
        case EV_OP_ENDED:
            printf("EV_OP_ENDED\n");
            break;
        case EV_INFO_LIB:
            printf("EV_INFO_LIB\n");
            printf("%s\n", (char*) voidPointer);
            break;
        case EV_INFO_FW:
            printf("EV_INFO_FW\n");
            printf("%s\n", (char*) voidPointer);
            break;
        case EV_INFO_PROGRESS_START:
            printf("EV_INFO_PROGRESS_START\n");
            printf("%" PRIu32 "\n", *((uint32_t*) voidPointer));
            break;
        case EV_INFO_PROGRESS:
            printf("EV_INFO_PROGRESS\n");
            printf("%" PRIu32 "\n", *((uint32_t*) voidPointer));
            break;
        case EV_FAIL_COMM_TX:
            printf("EV_FAIL_COMM_TX\n");
            break;
        case EV_FAIL_COMM_TIMEOUT:
            printf("EV_FAIL_COMM_TIMEOUT\n");
            break;
        case EV_FAIL_WRONG_UNIT_CONNECTED:
            printf("EV_FAIL_WRONG_UNIT_CONNECTED\n");
            break;
        case EV_FAIL_INVALID_SETTINGS:
            printf("EV_FAIL_INVALID_SETTINGS\n");
            break;
        case EV_FAIL_INCOMPATIBLE_HEX_FILE:
            printf("EV_FAIL_INCOMPATIBLE_HEX_FILE\n");
            break;
        case EV_FAIL_FILE_READ:
            printf("EV_FAIL_FILE_READ\n");
            break;
        case EV_FAIL_BL_PROT:
            printf("EV_FAIL_BL_PROT\n");
            break;
        case EV_RSP_WRITE_VER_FAIL:
            printf("EV_RSP_WRITE_VER_FAIL\n");
            break;
        case EV_RSP_COMM_CHECKSUM_FAIL:
            printf("EV_RSP_COMM_CHECKSUM_FAIL\n");
            break;
        case EV_RSP_OP_NOT_SUPPORTED:
            printf("EV_RSP_OP_NOT_SUPPORTED\n");
            break;
        case EV_RSP_BAD_PASSWORD:
            printf("EV_RSP_BAD_PASSWORD\n");
            break;
        case EV_RSP_BL_PROT_TRIP:
            printf("EV_RSP_BL_PROT_TRIP\n");
            break;
        case EV_RSP_BAD_DATA_LEN:
            printf("EV_RSP_BAD_DATA_LEN\n");
            break;
        case EV_RSP_BAD_MAC:
            printf("EV_RSP_BAD_MAC\n");
            break;
        case EV_RSP_UNKNOWN:
            printf("EV_RSP_UNKNOWN\n");
            break;
    }
    fflush(stdout);
}

#endif

int XIMU3_upload_firmware(const char* deviceName, const char* fileName, const char* portName)
{
    ds30_init();

    ds30_options_t options;
    ds30_set_defaults(&options);

    options.bl_type = BL_DS30_LOADER;
    options.device_name = deviceName;
    options.file_name = fileName;
    options.comm_type = COMM_UART;
    options.port_name = portName;
    options.baud_rate = 115200;
    options.write_flash = true;

#ifdef DEBUG_MODE
    options.event_callback = callback;
    options.debug_mode = true;

    printf("%s\n", fileName);
    printf("%s\n", portName);
#endif

    return ds30_write(&options);
}
