#pragma once

#include "../../C/Ximu3.h"
#include "EventArgs.h"
#include "NetworkAnnouncementMessage.h"

using namespace System::Runtime::InteropServices;

namespace Ximu3
{
    public ref class NetworkAnnouncement
    {
    public:
        event EventHandler<NetworkAnnouncementEventArgs^>^ NetworkAnnouncementEvent;

    public:
        NetworkAnnouncement()
        {
            networkAnnouncement = ximu3::XIMU3_network_announcement_new();
            ximu3::XIMU3_network_announcement_add_callback(networkAnnouncement, static_cast<ximu3::XIMU3_CallbackNetworkAnnouncementMessageC>(Marshal::GetFunctionPointerForDelegate(networkAnnouncementsDelegate).ToPointer()), GCHandle::ToIntPtr(thisHandle).ToPointer());
        }

        ~NetworkAnnouncement()
        {
            ximu3::XIMU3_network_announcement_free(networkAnnouncement);
        }

        Result GetResult()
        {
            return (Result)ximu3::XIMU3_network_announcement_get_result(networkAnnouncement);
        }

        array<NetworkAnnouncementMessage^>^ GetMessages()
        {
            return NetworkAnnouncementMessage::ToArrayAndFree(ximu3::XIMU3_network_announcement_get_messages(networkAnnouncement));
        }

        array<NetworkAnnouncementMessage^>^ GetMessagesAfterShortDelay()
        {
            return NetworkAnnouncementMessage::ToArrayAndFree(ximu3::XIMU3_network_announcement_get_messages_after_short_delay(networkAnnouncement));
        }

    private:
        ximu3::XIMU3_NetworkAnnouncement* networkAnnouncement;

        GCHandle thisHandle = GCHandle::Alloc(this, GCHandleType::Weak);

        delegate void NetworkAnnouncementDelegate(ximu3::XIMU3_NetworkAnnouncementMessage data, void* context);

        static void NetworkAnnouncementCallback(ximu3::XIMU3_NetworkAnnouncementMessage data, void* context)
        {
            auto sender = GCHandle::FromIntPtr(IntPtr(context)).Target;
            static_cast<NetworkAnnouncement^>(sender)->NetworkAnnouncementEvent(sender, gcnew NetworkAnnouncementEventArgs(gcnew NetworkAnnouncementMessage(data)));
        }

        const NetworkAnnouncementDelegate^ networkAnnouncementsDelegate = gcnew NetworkAnnouncementDelegate(NetworkAnnouncementCallback);
    };
}
