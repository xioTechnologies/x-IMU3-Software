using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class PortScanner(PortScanner.Callback callback) : IDisposable
    {
        public delegate void Callback(CApi.XIMU3_Device[] devices);

        private static void CallbackInternal(CApi.XIMU3_Devices devices, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<Callback>(context)(Helpers.ToArrayAndFree(devices));
        }

        ~PortScanner() => Dispose();

        public void Dispose()
        {
            if (wrapped != IntPtr.Zero)
            {
                CApi.XIMU3_port_scanner_free(wrapped);
                wrapped = IntPtr.Zero;
            }

            GC.SuppressFinalize(this);
        }

        public CApi.XIMU3_Device[] GetDevices()
        {
            return Helpers.ToArrayAndFree(CApi.XIMU3_port_scanner_get_devices(wrapped));
        }

        public static CApi.XIMU3_Device[] Scan()
        {
            return Helpers.ToArrayAndFree(CApi.XIMU3_port_scanner_scan());
        }

        public static CApi.XIMU3_Device[] ScanFilter(CApi.XIMU3_PortType portType)
        {
            return Helpers.ToArrayAndFree(CApi.XIMU3_port_scanner_scan_filter(portType));
        }

        public static string[] GetPortNames()
        {
            return ToArrayAndFree(CApi.XIMU3_port_scanner_get_port_names());
        }

        private static string[] ToArrayAndFree(CApi.XIMU3_CharArrays arrays)
        {
            var array = new string[arrays.length];

            for (var i = 0; i < arrays.length; i++)
            {
                array[i] = Marshal.PtrToStringAnsi(arrays.array + i * CApi.XIMU3_CHAR_ARRAY_SIZE)!;
            }

            CApi.XIMU3_char_arrays_free(arrays);

            return array;
        }

        private IntPtr wrapped = CApi.XIMU3_port_scanner_new(CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
    }
}
