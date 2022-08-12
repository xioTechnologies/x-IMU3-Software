#ifndef __linux__ // until provided bootloader library for Linux

#include "ds30_loader.h"

#endif

#include "Ximu3Bootloader.h"

int XIMU3_upload_firmware(const char* fileName, const char* portName)
{
#ifndef __linux__ // until provided bootloader library for Linux
    ds30_init();

    ds30_options_t options;
    ds30_set_defaults(&options);

    options.file_name = fileName;
    options.port_name = portName;
    options.baud_rate = 115200;
    options.device_name = "PIC32MZ2048EFG124";
    options.comm_type = DS30_UART;
    options.write_flash = true;

    return ds30_write(&options);
#else
    return 1;
#endif
}
