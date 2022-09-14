#ifndef XIMU3_BOOTLOADER_H
#define XIMU3_BOOTLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

int XIMU3_upload_firmware(const char* deviceName, const char* fileName, const char* portName);

#ifdef __cplusplus
}
#endif

#endif
