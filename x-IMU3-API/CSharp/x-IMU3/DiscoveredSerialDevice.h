#pragma once

#include "../../C/Ximu3.h"
#include "ConnectionInfo.h"

using namespace System;

namespace Ximu3
{
    public ref class DiscoveredSerialDevice
    {
    internal:
        DiscoveredSerialDevice(ximu3::XIMU3_DiscoveredSerialDevice device) : device{ new ximu3::XIMU3_DiscoveredSerialDevice{device} }
        {
        }

    public:
        ~DiscoveredSerialDevice()
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
            return gcnew String(ximu3::XIMU3_discovered_serial_device_to_string(*device));
        }

    internal:
        static array<DiscoveredSerialDevice^>^ ToArrayAndFree(const ximu3::XIMU3_DiscoveredSerialDevices devices) {
            array<DiscoveredSerialDevice^>^ devicesArray = gcnew array<DiscoveredSerialDevice^>(devices.length);

            for (uint32_t index = 0; index < devices.length; index++)
            {
                devicesArray[index] = gcnew DiscoveredSerialDevice(devices.array[index]);
            }

            ximu3::XIMU3_discovered_serial_devices_free(devices);
            return devicesArray;
        }

    private:
        ximu3::XIMU3_DiscoveredSerialDevice* device;
    };
}
