#pragma once

#include "../../C/Ximu3.h"
#include "ConnectionType.h"
#include "DiscoveredSerialDevice.h"
#include "EventArgs.h"
#include "Helpers.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Ximu3
{
    public ref class SerialDiscovery
    {
    public:
        SerialDiscovery(EventHandler<SerialDiscoveryEventArgs^>^ serialDiscoveryEvent) : serialDiscoveryEvent{ serialDiscoveryEvent }
        {
            discovery = ximu3::XIMU3_serial_discovery_new(static_cast<ximu3::XIMU3_CallbackDiscoveredSerialDevices>(Marshal::GetFunctionPointerForDelegate(serialDiscoveryDelegate).ToPointer()), GCHandle::ToIntPtr(thisHandle).ToPointer());
        }

        ~SerialDiscovery() {
            ximu3::XIMU3_serial_discovery_free(discovery);
        }

        array<DiscoveredSerialDevice^>^ GetDevices()
        {
            return DiscoveredSerialDevice::ToArrayAndFree(ximu3::XIMU3_serial_discovery_get_devices(discovery));
        }

        static array<DiscoveredSerialDevice^>^ Scan(int milliseconds)
        {
            return DiscoveredSerialDevice::ToArrayAndFree(ximu3::XIMU3_serial_discovery_scan(milliseconds));
        }

        static array<DiscoveredSerialDevice^>^ ScanFilter(int milliseconds, ConnectionType connectionType)
        {
            return DiscoveredSerialDevice::ToArrayAndFree(ximu3::XIMU3_serial_discovery_scan_filter(milliseconds, (ximu3::XIMU3_ConnectionType)connectionType));
        }

        static array<String^>^ GetAvailablePorts()
        {
            return Helpers::ToArrayAndFree(ximu3::XIMU3_serial_discovery_get_available_ports());
        }

    private:
        ximu3::XIMU3_SerialDiscovery* discovery;

        GCHandle thisHandle = GCHandle::Alloc(this, GCHandleType::Weak);

        EventHandler<SerialDiscoveryEventArgs^>^ serialDiscoveryEvent;

        delegate void CallbackDelegate(ximu3::XIMU3_DiscoveredSerialDevices data, void* context);

        static void Callback(ximu3::XIMU3_DiscoveredSerialDevices data, void* context)
        {
            auto sender = GCHandle::FromIntPtr(IntPtr(context)).Target;
            static_cast<SerialDiscovery^>(sender)->serialDiscoveryEvent(sender, gcnew SerialDiscoveryEventArgs(DiscoveredSerialDevice::ToArrayAndFree(data)));
        }

        const CallbackDelegate^ serialDiscoveryDelegate = gcnew CallbackDelegate(Callback);
    };
}
