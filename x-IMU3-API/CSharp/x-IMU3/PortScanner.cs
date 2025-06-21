using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class PortScanner(PortScanner.Callback callback) : IDisposable
    {
        public delegate void Callback(CApi.XIMU3_Device[] devices);
        private static void CallbackInternal(CApi.XIMU3_Devices devices, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<Callback>(context)(ToArrayAndFree(devices));
        }

        ~PortScanner() => Dispose();

        public void Dispose()
        {
            if (portScanner != IntPtr.Zero)
            {
                CApi.XIMU3_port_scanner_free(portScanner);
                portScanner = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        public CApi.XIMU3_Device[] GetDevices()
        {
            return ToArrayAndFree(CApi.XIMU3_port_scanner_get_devices(portScanner));
        }

        public static CApi.XIMU3_Device[] Scan()
        {
            return ToArrayAndFree(CApi.XIMU3_port_scanner_scan());
        }

        public static CApi.XIMU3_Device[] ScanFilter(CApi.XIMU3_ConnectionType connectionType)
        {
            return ToArrayAndFree(CApi.XIMU3_port_scanner_scan_filter(PortType));
        }

        public static string[] GetPortNames()
        {
            return Helpers.ToArrayAndFree(CApi.XIMU3_port_scanner_get_port_names());
        }

        private static CApi.XIMU3_Device[] ToArrayAndFree(CApi.XIMU3_Devices devices_)
        {
            CApi.XIMU3_Device[] devices = new CApi.XIMU3_Device[devices_.length];
            for (int i = 0; i < devices_.length; i++)
            {
                devices[i] = Marshal.PtrToStructure<CApi.XIMU3_Device>(devices_.array + i * Marshal.SizeOf(typeof(CApi.XIMU3_Device)));
            }
            CApi.XIMU3_devices_free(devices_);
            return devices;
        }

        private IntPtr portScanner = CApi.XIMU3_port_scanner_new(CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
    }
}