using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class DataLogger : IDisposable
    {
        public DataLogger(string destination, string name, Connection[] connections)
        {
            wrapped = CApi.XIMU3_data_logger_new(
                Helpers.ToPointer(destination),
                Helpers.ToPointer(name),
                Marshal.UnsafeAddrOfPinnedArrayElement(ToPointers(connections), 0),
                (UInt32)connections.Length
            );
        }

        ~DataLogger() => Dispose();

        public void Dispose()
        {
            if (wrapped != IntPtr.Zero)
            {
                CApi.XIMU3_data_logger_free(wrapped);

                wrapped = IntPtr.Zero;
            }

            GC.SuppressFinalize(this);
        }

        public CApi.XIMU3_Result GetResult()
        {
            return CApi.XIMU3_data_logger_get_result(wrapped);
        }

        public static CApi.XIMU3_Result Log(string destination, string name, Connection[] connections, UInt32 seconds)
        {
            return CApi.XIMU3_data_logger_log(
                Helpers.ToPointer(destination),
                Helpers.ToPointer(name),
                Marshal.UnsafeAddrOfPinnedArrayElement(ToPointers(connections), 0),
                (UInt32)connections.Length,
                seconds
            );
        }

        private static IntPtr[] ToPointers(Connection[] connections)
        {
            var pointers = new IntPtr[connections.Length];

            for (var i = 0; i < connections.Length; i++)
            {
                pointers[i] = connections[i].wrapped;
            }

            return pointers;
        }

        private IntPtr wrapped;
    }
}
