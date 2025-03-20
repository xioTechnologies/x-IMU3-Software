using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class FileConverter : IDisposable
    {
        public FileConverter(string destination, string name, string[] files, Callback callback)
        {
            fileConverter = CApi.XIMU3_file_converter_new(Helpers.ToPointer(destination), Helpers.ToPointer(name), Marshal.UnsafeAddrOfPinnedArrayElement(Helpers.ToPointers(files), 0), (UInt32)files.Length, CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public delegate void Callback(CApi.XIMU3_FileConverterProgress progress);
        private static void CallbackInternal(CApi.XIMU3_FileConverterProgress progress, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<Callback>(context)(progress);
        }

        ~FileConverter() => Dispose();

        public void Dispose()
        {
            if (fileConverter != IntPtr.Zero)
            {
                CApi.XIMU3_file_converter_free(fileConverter);
                fileConverter = IntPtr.Zero;
            }
            GC.SuppressFinalize(this);
        }

        public static CApi.XIMU3_FileConverterProgress Convert(string destination, string name, string[] files)
        {
            return CApi.XIMU3_file_converter_convert(Helpers.ToPointer(destination), Helpers.ToPointer(name), Marshal.UnsafeAddrOfPinnedArrayElement(Helpers.ToPointers(files), 0), (UInt32)files.Length);
        }

        private IntPtr fileConverter;

    }
}