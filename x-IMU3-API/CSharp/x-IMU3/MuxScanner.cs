using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class MuxScanner(Connection connection, MuxScanner.Callback callback) : IDisposable
    {
        public delegate void Callback(Device[] devices);

        private static void CallbackInternal(CApi.XIMU3_Devices devices, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<Callback>(context)(Helpers.ToArrayAndFree(devices));
        }

        ~MuxScanner() => Dispose();

        public void Dispose()
        {
            if (wrapped != IntPtr.Zero)
            {
                CApi.XIMU3_port_scanner_free(wrapped);
                wrapped = IntPtr.Zero;
            }

            GC.SuppressFinalize(this);
        }

        public Device[] GetDevices()
        {
            return Helpers.ToArrayAndFree(CApi.XIMU3_mux_scanner_get_devices(wrapped));
        }

        public static Device[] Scan(Connection connection, UInt32 numberOfChannels = CApi.XIMU3_MAX_NUMBER_OF_MUX_CHANNELS, UInt32 retries = CApi.XIMU3_DEFAULT_RETRIES, UInt32 timeout = CApi.XIMU3_DEFAULT_TIMEOUT)
        {
            return Helpers.ToArrayAndFree(CApi.XIMU3_mux_scanner_scan(connection.wrapped, numberOfChannels, retries, timeout));
        }

        private IntPtr wrapped = CApi.XIMU3_mux_scanner_new(connection.wrapped, CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
    }
}
