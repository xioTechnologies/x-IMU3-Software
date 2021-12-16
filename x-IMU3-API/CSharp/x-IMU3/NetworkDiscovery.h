#pragma once

#include "../../C/Ximu3.h"
#include "DiscoveredNetworkDevice.h"
#include "EventArgs.h"

using namespace System::Runtime::InteropServices;

namespace Ximu3
{
    public ref class NetworkDiscovery
    {
    public:
        NetworkDiscovery(EventHandler<NetworkDiscoveryEventArgs^>^ networkDiscoveryEvent) : networkDiscoveryEvent{ networkDiscoveryEvent }
        {
            discovery = ximu3::XIMU3_network_discovery_new(static_cast<ximu3::XIMU3_CallbackDiscoveredNetworkDevices>(Marshal::GetFunctionPointerForDelegate(networkDiscoveryDelegate).ToPointer()), GCHandle::ToIntPtr(thisHandle).ToPointer());
        }

        ~NetworkDiscovery() {
            ximu3::XIMU3_network_discovery_free(discovery);
        }

        array<DiscoveredNetworkDevice^>^ GetDevices()
        {
            return DiscoveredNetworkDevice::ToArrayAndFree(ximu3::XIMU3_network_discovery_get_devices(discovery));
        }

        static array<DiscoveredNetworkDevice^>^ Scan(int milliseconds)
        {
            return DiscoveredNetworkDevice::ToArrayAndFree(ximu3::XIMU3_network_discovery_scan(milliseconds));
        }

    private:
        ximu3::XIMU3_NetworkDiscovery* discovery;

        GCHandle thisHandle = GCHandle::Alloc(this, GCHandleType::Weak);

        EventHandler<NetworkDiscoveryEventArgs^>^ networkDiscoveryEvent;

        delegate void CallbackDelegate(ximu3::XIMU3_DiscoveredNetworkDevices data, void* context);

        static void Callback(ximu3::XIMU3_DiscoveredNetworkDevices data, void* context)
        {
            auto sender = GCHandle::FromIntPtr(IntPtr(context)).Target;
            static_cast<NetworkDiscovery^>(sender)->networkDiscoveryEvent(sender, gcnew NetworkDiscoveryEventArgs(DiscoveredNetworkDevice::ToArrayAndFree(data)));
        }

        const CallbackDelegate^ networkDiscoveryDelegate = gcnew CallbackDelegate(Callback);
    };
}
