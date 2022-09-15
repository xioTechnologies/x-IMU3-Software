#pragma once

#include "../../C/Ximu3.h"
#include "ConnectionType.h"
#include "Device.h"
#include "EventArgs.h"
#include "Helpers.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Ximu3
{
	public ref class PortScanner
	{
	public:
		PortScanner(EventHandler<PortScannerEventArgs^>^ portScannerEvent) : portScannerEvent{ portScannerEvent }
		{
			portScanner = ximu3::XIMU3_port_scanner_new(static_cast<ximu3::XIMU3_CallbackDevices>(Marshal::GetFunctionPointerForDelegate(portScannerDelegate).ToPointer()), GCHandle::ToIntPtr(thisHandle).ToPointer());
		}

		~PortScanner() {
			ximu3::XIMU3_port_scanner_free(portScanner);
		}

		array<Device^>^ GetDevices()
		{
			return Device::ToArrayAndFree(ximu3::XIMU3_port_scanner_get_devices(portScanner));
		}

		static array<Device^>^ Scan()
		{
			return Device::ToArrayAndFree(ximu3::XIMU3_port_scanner_scan());
		}

		static array<Device^>^ ScanFilter(ConnectionType connectionType)
		{
			return Device::ToArrayAndFree(ximu3::XIMU3_port_scanner_scan_filter((ximu3::XIMU3_ConnectionType)connectionType));
		}

		static array<String^>^ GetPortNames()
		{
			return Helpers::ToArrayAndFree(ximu3::XIMU3_port_scanner_get_port_names());
		}

	private:
		ximu3::XIMU3_PortScanner* portScanner;

		GCHandle thisHandle = GCHandle::Alloc(this, GCHandleType::Weak);

		EventHandler<PortScannerEventArgs^>^ portScannerEvent;

		delegate void CallbackDelegate(ximu3::XIMU3_Devices data, void* context);

		static void Callback(ximu3::XIMU3_Devices data, void* context)
		{
			auto sender = GCHandle::FromIntPtr(IntPtr(context)).Target;
			static_cast<PortScanner^>(sender)->portScannerEvent(sender, gcnew PortScannerEventArgs(Device::ToArrayAndFree(data)));
		}

		const CallbackDelegate^ portScannerDelegate = gcnew CallbackDelegate(Callback);
	};
}
