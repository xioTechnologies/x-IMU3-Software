using System;
using System.Runtime.InteropServices;

namespace Ximu3
{
    public class NetworkAnnouncement : IDisposable
    {
        public NetworkAnnouncement()
        {
            networkAnnouncement = CApi.XIMU3_network_announcement_new();
        }

        ~NetworkAnnouncement() => Dispose();

        public void Dispose()
        {
            if (networkAnnouncement != IntPtr.Zero)
            {
                CApi.XIMU3_network_announcement_free(networkAnnouncement);

                networkAnnouncement = IntPtr.Zero;
            }

            GC.SuppressFinalize(this);
        }

        public CApi.XIMU3_Result GetResult()
        {
            return CApi.XIMU3_network_announcement_get_result(networkAnnouncement);
        }

        public delegate void Callback(CApi.XIMU3_NetworkAnnouncementMessage message);

        private static void CallbackInternal(CApi.XIMU3_NetworkAnnouncementMessage message, IntPtr context)
        {
            Marshal.GetDelegateForFunctionPointer<Callback>(context)(message);
        }

        public UInt64 AddCallback(Callback callback)
        {
            return CApi.XIMU3_network_announcement_add_callback(networkAnnouncement, CallbackInternal, Marshal.GetFunctionPointerForDelegate(callback));
        }

        public void RemoveCallback(UInt64 callbackId)
        {
            CApi.XIMU3_network_announcement_remove_callback(networkAnnouncement, callbackId);
        }

        public CApi.XIMU3_NetworkAnnouncementMessage[] GetMessages()
        {
            return ToArrayAndFree(CApi.XIMU3_network_announcement_get_messages(networkAnnouncement));
        }

        public CApi.XIMU3_NetworkAnnouncementMessage[] GetMessagesAfterShortDelay()
        {
            return ToArrayAndFree(CApi.XIMU3_network_announcement_get_messages_after_short_delay(networkAnnouncement));
        }

        private static CApi.XIMU3_NetworkAnnouncementMessage[] ToArrayAndFree(CApi.XIMU3_NetworkAnnouncementMessages messages)
        {
            var array = new CApi.XIMU3_NetworkAnnouncementMessage[messages.length];

            for (var i = 0; i < messages.length; i++)
            {
                array[i] = Marshal.PtrToStructure<CApi.XIMU3_NetworkAnnouncementMessage>(messages.array + i * Marshal.SizeOf(typeof(CApi.XIMU3_NetworkAnnouncementMessage)));
            }

            CApi.XIMU3_network_announcement_messages_free(messages);

            return array;
        }

        private IntPtr networkAnnouncement;
    }
}
