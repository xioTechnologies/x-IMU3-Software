using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class KeepOpen : IDisposable
    {
        public KeepOpen(Connection connection, Callback callback)
        {
            keepOpen = CApi.XIMU3_keep_open_new(connection.connection, CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void Callback(CApi.XIMU3_ConnectionStatus status);
        private static void CallbackInternal(CApi.XIMU3_ConnectionStatus status, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<Callback>(context)(status);
        }

        ~KeepOpen() => Dispose();

        public void Dispose()
        {
            if (keepOpen != IntPtr.Zero)
            {
                CApi.XIMU3_keep_open_free(keepOpen);
                keepOpen = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        private IntPtr keepOpen;
    }
}