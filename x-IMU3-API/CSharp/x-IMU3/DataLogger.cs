using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class DataLogger : IDisposable
    {
        public DataLogger(string destination, string name, Connection[] connections)
        {
            dataLogger = CApi.XIMU3_data_logger_new(Helpers.ToPointer(destination), Helpers.ToPointer(name), Marshal.UnsafeAddrOfPinnedArrayElement(ToPointers(connections), 0), (UInt32)connections.Length);
        }

        ~DataLogger() => Dispose();

        public void Dispose()
        {
            if (dataLogger != IntPtr.Zero)
            {
                CApi.XIMU3_data_logger_free(dataLogger);
                dataLogger = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        public CApi.XIMU3_Result GetResult()
        {
            return CApi.XIMU3_data_logger_get_result(dataLogger);
        }

        public static CApi.XIMU3_Result Log(string destination, string name, Connection[] connections, UInt32 seconds)
        {
            return CApi.XIMU3_data_logger_log(Helpers.ToPointer(destination), Helpers.ToPointer(name), Marshal.UnsafeAddrOfPinnedArrayElement(ToPointers(connections), 0), (UInt32)connections.Length, seconds);
        }

        private static IntPtr[] ToPointers(Connection[] connections)
        {
            IntPtr[] pointers = new IntPtr[connections.Length];
            for (int i = 0; i < connections.Length; i++)
            {
                pointers[i] = connections[i].connection;
            }
            return pointers;
        }

        private IntPtr dataLogger;
    }
}