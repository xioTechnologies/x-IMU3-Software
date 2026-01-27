using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class KeepOpen(Connection connection, KeepOpen.Callback callback) : IDisposable
    {
        public delegate void Callback(CApi.XIMU3_ConnectionStatus status);

        private static void CallbackInternal(CApi.XIMU3_ConnectionStatus status, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<Callback>(context)(status);
        }

        ~KeepOpen() => Dispose();

        public void Dispose()
        {
            if (wrapped != IntPtr.Zero)
            {
                CApi.XIMU3_keep_open_free(wrapped);

                wrapped = IntPtr.Zero;
            }

            GC.SuppressFinalize(this);
        }

        private IntPtr wrapped = CApi.XIMU3_keep_open_new(connection.wrapped, CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
    }
}
