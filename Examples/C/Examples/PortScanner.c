#include "../../../x-IMU3-API/C/Ximu3.h"
#include "../Helpers.h"
#include <stdio.h>

static void Callback(const XIMU3_Devices devices, void* context);

static void PrintDevices(const XIMU3_Devices devices);

void PortScanner()
{
    if (YesOrNo("Use async implementation?") == true)
    {
        XIMU3_PortScanner* const portScanner = XIMU3_port_scanner_new(Callback, NULL);
        Wait(60);
        XIMU3_port_scanner_free(portScanner);
    }
    else
    {
        const XIMU3_Devices devices = XIMU3_port_scanner_scan();
        printf("Found %u devices\n", devices.length);
        PrintDevices(devices);
        XIMU3_devices_free(devices);
    }
}

static void Callback(const XIMU3_Devices devices, void* context)
{
    printf("Devices updated (%u devices available)\n", devices.length);
    PrintDevices(devices);
    XIMU3_devices_free(devices);
}

static void PrintDevices(const XIMU3_Devices devices)
{
    for (uint32_t index = 0; index < devices.length; index++)
    {
        const XIMU3_Device* const device = &devices.array[index];
        const char* connectionInfo;
        switch (device->connection_type)
        {
            case XIMU3_ConnectionTypeUsb:
                connectionInfo = XIMU3_usb_connection_info_to_string(device->usb_connection_info);
                break;
            case XIMU3_ConnectionTypeSerial:
                connectionInfo = XIMU3_serial_connection_info_to_string(device->serial_connection_info);
                break;
            case XIMU3_ConnectionTypeBluetooth:
                connectionInfo = XIMU3_bluetooth_connection_info_to_string(device->bluetooth_connection_info);
                break;
            default:
                connectionInfo = "";
                break;
        }
        printf("%s, %s, %s\n",
               device->device_name,
               device->serial_number,
               connectionInfo);
        // printf("%s\n", XIMU3_device_to_string(*device)); // alternative to above
    }
}
