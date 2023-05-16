#pragma once

#include "../C/Ximu3.h"
#include <functional>
#include "Helpers.hpp"
#include <string>
#include <vector>

namespace ximu3
{
    class NetworkAnnouncement
    {
    public:
        NetworkAnnouncement()
        {
            networkAnnouncement = XIMU3_network_announcement_new();
        }

        ~NetworkAnnouncement()
        {
            XIMU3_network_announcement_free(networkAnnouncement);
        }

        XIMU3_Result getResult()
        {
            return XIMU3_network_announcement_get_result(networkAnnouncement);
        }

        uint64_t addCallback(std::function<void(XIMU3_NetworkAnnouncementMessage)>& callback)
        {
            return XIMU3_network_announcement_add_callback(networkAnnouncement, Helpers::wrapCallable<XIMU3_NetworkAnnouncementMessage>(callback), &callback);
        }

        void removeCallback(const uint64_t callbackID)
        {
            XIMU3_network_announcement_remove_callback(networkAnnouncement, callbackID);
        }

        std::vector<XIMU3_NetworkAnnouncementMessage> getMessages()
        {
            return toVectorAndFree(XIMU3_network_announcement_get_messages(networkAnnouncement));
        }

        std::vector<XIMU3_NetworkAnnouncementMessage> getMessagesAfterShortDelay()
        {
            return toVectorAndFree(XIMU3_network_announcement_get_messages_after_short_delay(networkAnnouncement));
        }

    private:
        XIMU3_NetworkAnnouncement* networkAnnouncement;

        static std::vector<XIMU3_NetworkAnnouncementMessage> toVectorAndFree(const XIMU3_NetworkAnnouncementMessages& messages)
        {
            const std::vector<XIMU3_NetworkAnnouncementMessage> vector = Helpers::toVector<XIMU3_NetworkAnnouncementMessage>(messages);
            XIMU3_network_announcement_messages_free(messages);
            return vector;
        }
    };
} // namespace ximu3
