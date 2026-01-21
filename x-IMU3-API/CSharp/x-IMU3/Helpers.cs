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

        internal static string[] ToArrayAndFree(CApi.XIMU3_CharArrays arrays)
        {
            var array = new string[arrays.length];

            for (var i = 0; i < arrays.length; i++)
            {
                array[i] = Marshal.PtrToStringAnsi(arrays.array + i * CApi.XIMU3_CHAR_ARRAY_SIZE)!;
            }

            CApi.XIMU3_char_arrays_free(arrays);

            return array;
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
    }
}
