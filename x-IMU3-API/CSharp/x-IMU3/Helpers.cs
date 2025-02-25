using System.Runtime.InteropServices;
using System.Text;

namespace Ximu3
{
    public static class Helpers
    {
        public static string ToString(byte[] bytes)
        {
            GCHandle handle = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            string result = Marshal.PtrToStringAnsi(handle.AddrOfPinnedObject())!;
            handle.Free();
            return result;
        }

        public static string ToString(IntPtr charPtr)
        {
            return Marshal.PtrToStringAnsi(charPtr)!;
        }

        public static byte[] ToBytes(string string_)
        {
            byte[] bytes = new byte[CApi.XIMU3_CHAR_ARRAY_SIZE];
            Array.Copy(Encoding.ASCII.GetBytes(string_), bytes, Math.Min(string_.Length, CApi.XIMU3_CHAR_ARRAY_SIZE - 1));
            return bytes;
        }

        internal static string[] ToArrayAndFree(CApi.XIMU3_CharArrays arrays_)
        {
            string[] array = new string[arrays_.length];
            for (int i = 0; i < arrays_.length; i++)
            {
                array[i] = Marshal.PtrToStringAnsi(arrays_.array + i * CApi.XIMU3_CHAR_ARRAY_SIZE)!;
            }
            CApi.XIMU3_char_arrays_free(arrays_);
            return array;
        }

        public static IntPtr ToPointer(string string_)
        {
            return Marshal.StringToHGlobalAnsi(string_);
        }

        internal static IntPtr[] ToPointers(string[] strings)
        {
            IntPtr[] pointers = new IntPtr[strings.Length];
            for (int i = 0; i < strings.Length; i++)
            {
                pointers[i] = Marshal.StringToHGlobalAnsi(strings[i]);
            }
            return pointers;
        }
    }
}