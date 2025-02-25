using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

public static class Ximu3Api
{
    public const int XIMU3_DATA_MESSAGE_CHAR_ARRAY_SIZE = 256;
    public const int XIMU3_CHAR_ARRAY_SIZE = 256;

    public enum XIMU3_Result
    {
        Ok,
        Error,
    }

    public enum ChargingStatus
    {
        NotConnected,
        Charging,
        ChargingComplete,
        ChargingOnHold
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct UsbConnectionInfo
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
        public byte[] port_name;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PingResponse
    {
        public XIMU3_Result result;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
        public byte[] interface_;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
        public byte[] device_name;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = XIMU3_CHAR_ARRAY_SIZE)]
        public byte[] serial_number;
    }


    [StructLayout(LayoutKind.Sequential)]
    public struct XIMU3_Statistics
    {
        public ulong timestamp;
        public ulong data_total;
        public uint data_rate;
        public ulong message_total;
        public uint message_rate;
        public ulong error_total;
        public uint error_rate;
    }



    // Charging status
    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]

    public static extern ChargingStatus XIMU3_charging_status_from_float(float chargingStatus);

    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr XIMU3_charging_status_to_string(ChargingStatus chargingStatus);

    // Connection
    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr XIMU3_connection_new_usb(UsbConnectionInfo usbConnectionInfo);

    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern XIMU3_Result XIMU3_connection_open(IntPtr connection);

    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern PingResponse XIMU3_connection_ping(IntPtr connection);

    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern UsbConnectionInfo XIMU3_connection_get_info_usb(IntPtr connection);

    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern XIMU3_Statistics XIMU3_connection_get_statistics(IntPtr connection);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void CallbackStatistics(XIMU3_Statistics data, IntPtr context);

    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern ulong XIMU3_connection_add_statistics_callback(IntPtr connection, CallbackStatistics callback, IntPtr context);

    // Connection info
    [DllImport("ximu3.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr XIMU3_usb_connection_info_to_string(UsbConnectionInfo usbConnectionInfo);
}

namespace Ximu3Examples
{
    class Program
    {
        static void callback(Ximu3Api.XIMU3_Statistics data, IntPtr context)
        {
            Console.WriteLine("callback");
            Console.WriteLine(data.timestamp);
        }

        static int Main(string[] args)
        {
            // Create connection info
            Ximu3Api.UsbConnectionInfo usbConnectionInfo = new Ximu3Api.UsbConnectionInfo();
            usbConnectionInfo.port_name = new byte[Ximu3Api.XIMU3_CHAR_ARRAY_SIZE];
            string portName = "COM3";
            byte[] portNameBytes = Encoding.ASCII.GetBytes(portName);
            Array.Clear(usbConnectionInfo.port_name, 0, usbConnectionInfo.port_name.Length);
            Array.Copy(portNameBytes, usbConnectionInfo.port_name, portNameBytes.Length);
            Console.WriteLine(Marshal.PtrToStringAnsi(Ximu3Api.XIMU3_usb_connection_info_to_string(usbConnectionInfo)));

            // Create connection
            IntPtr connection = Ximu3Api.XIMU3_connection_new_usb(usbConnectionInfo);

            // Open and ping
            Ximu3Api.XIMU3_Result result = Ximu3Api.XIMU3_connection_open(connection);
            Ximu3Api.PingResponse pingResponse = Ximu3Api.XIMU3_connection_ping(connection);
            Console.WriteLine(Encoding.UTF8.GetString(pingResponse.device_name));

            // Add callback
            Ximu3Api.XIMU3_connection_add_statistics_callback(connection, callback, 0);

            Thread.Sleep(60000);

            

            Console.WriteLine("Press any key to exit");
            return 0;
        }
    }
}
