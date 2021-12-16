#pragma once

#include "../../C/Ximu3.h"
#include "ChargingStatus.h"
#include "ConnectionInfo.h"

using namespace System;

namespace Ximu3
{
    public ref class DiscoveredNetworkDevice
    {
    internal:
        DiscoveredNetworkDevice(ximu3::XIMU3_DiscoveredNetworkDevice device) : device{ new ximu3::XIMU3_DiscoveredNetworkDevice{device} }
        {
        }

    public:
        ~DiscoveredNetworkDevice()
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

        property int32_t Rssi
        {
            int32_t get()
            {
                return device->rssi;
            }
        }

        property int32_t Battery
        {
            int32_t get()
            {
                return device->battery;
            }
        }

        property ChargingStatus Status
        {
            ChargingStatus get()
            {
                return (ChargingStatus)device->status;
            }
        }

        property TcpConnectionInfo^ TcpConnectionInfo
        {
            Ximu3::TcpConnectionInfo^ get()
            {
                return gcnew Ximu3::TcpConnectionInfo(&device->tcp_connection_info);
            }
        }

        property UdpConnectionInfo^ UdpConnectionInfo
        {
            Ximu3::UdpConnectionInfo^ get()
            {
                return gcnew Ximu3::UdpConnectionInfo(&device->udp_connection_info);
            }
        }

        String^ ToString() override
        {
            return gcnew String(ximu3::XIMU3_discovered_network_device_to_string(*device));
        }

    internal:
        static array<DiscoveredNetworkDevice^>^ ToArrayAndFree(const ximu3::XIMU3_DiscoveredNetworkDevices devices) {
            array<DiscoveredNetworkDevice^>^ devicesArray = gcnew array<DiscoveredNetworkDevice^>(devices.length);

            for (uint32_t index = 0; index < devices.length; index++)
            {
                devicesArray[index] = gcnew DiscoveredNetworkDevice(devices.array[index]);
            }

            ximu3::XIMU3_discovered_network_devices_free(devices);
            return devicesArray;
        }

    private:
        ximu3::XIMU3_DiscoveredNetworkDevice* device;
    };
}
