#pragma once

#include "../../C/Ximu3.h"
#include "ConnectionInfo.h"

using namespace System;

namespace Ximu3
{
    public ref class Device
    {
    internal:
        Device(ximu3::XIMU3_Device device) : device{ new ximu3::XIMU3_Device{device} }
        {
        }

    public:
        ~Device()
        {
            delete device;
        }

        property String^ DeviceName
        {
            String^ get()
            {
                return gcnew String(device->device_name);
            }
        }

        property String^ SerialNumber
        {
            String^ get()
            {
                return gcnew String(device->serial_number);
            }
        }

        property ConnectionInfo^ ConnectionInfo
        {
            Ximu3::ConnectionInfo^ get()
            {
                switch (device->connection_type)
                {
                case ximu3::XIMU3_ConnectionTypeUsb:
                    return gcnew UsbConnectionInfo(&device->usb_connection_info);
                case ximu3::XIMU3_ConnectionTypeSerial:
                    return gcnew SerialConnectionInfo(&device->serial_connection_info);
                case ximu3::XIMU3_ConnectionTypeBluetooth:
                    return gcnew BluetoothConnectionInfo(&device->bluetooth_connection_info);
                default:
                    return nullptr;
                }
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_device_to_string(*device));
        }

    internal:
        static array<Device^>^ ToArrayAndFree(const ximu3::XIMU3_Devices devices) {
            array<Device^>^ devicesArray = gcnew array<Device^>(devices.length);

            for (uint32_t index = 0; index < devices.length; index++)
            {
                devicesArray[index] = gcnew Device(devices.array[index]);
            }

            ximu3::XIMU3_devices_free(devices);
            return devicesArray;
        }

    private:
        ximu3::XIMU3_Device* device;
    };
}
