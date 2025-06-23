using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class Demux(Connection connection, Byte[] channels) : IDisposable
    {
        ~Demux() => Dispose();

        public void Dispose()
        {
            if (demux != IntPtr.Zero)
            {
                CApi.XIMU3_demux_free(demux);
                demux = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        public CApi.XIMU3_Result GetResult()
        {
            return CApi.XIMU3_demux_get_result(demux);
        }

        private IntPtr demux = CApi.XIMU3_demux_new(connection.connection, Marshal.UnsafeAddrOfPinnedArrayElement(channels, 0), (UInt32)channels.Length);
    }
}
