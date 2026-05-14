using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class KeepOpen(Connection connection) : IDisposable
    {
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

        private IntPtr wrapped = CApi.XIMU3_keep_open_new(connection.wrapped);
    }
}
