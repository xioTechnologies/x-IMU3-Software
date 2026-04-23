using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Ximu3
{
    public static class Helpers
    {
        public static string ToString(byte[] bytes)
        {
            var handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);

            var result = Marshal.PtrToStringAnsi(handle.AddrOfPinnedObject())!;

            handle.Free();

            return result;
        }

        public static string ToString(IntPtr charPtr)
        {
            return Marshal.PtrToStringAnsi(charPtr)!;
        }

        public static byte[] ToBytes(string @string)
        {
            var bytes = new byte[CApi.XIMU3_CHAR_ARRAY_SIZE];

            Array.Copy(Encoding.ASCII.GetBytes(@string), bytes, Math.Min(@string.Length, CApi.XIMU3_CHAR_ARRAY_SIZE - 1));

            return bytes;
        }

        public static IntPtr ToPointer(string @string)
        {
            return Marshal.StringToHGlobalAnsi(@string);
        }

        internal static IntPtr[] ToPointers(string[] strings)
        {
            var pointers = new IntPtr[strings.Length];

            for (var i = 0; i < strings.Length; i++)
            {
                pointers[i] = Marshal.StringToHGlobalAnsi(strings[i]);
            }

            return pointers;
        }

        internal static Device[] ToArrayAndFree(CApi.XIMU3_Devices devices)
        {
            var array = new Device[devices.length];

            for (var i = 0; i < devices.length; i++)
            {
                array[i] = new Device(Marshal.PtrToStructure<CApi.XIMU3_Device>(devices.array + i * Marshal.SizeOf(typeof(CApi.XIMU3_Device))));
            }

            CApi.XIMU3_devices_free(devices);

            return array;
        }
    }
}
