#pragma once

#include "../C/Ximu3.h"
#include <functional>
#include "Helpers.hpp"
#include <string>
#include <vector>

namespace ximu3 {
    class NetworkAnnouncement {
    public:
        NetworkAnnouncement() {
            wrapped = XIMU3_network_announcement_new();
        }

        ~NetworkAnnouncement() {
            XIMU3_network_announcement_free(wrapped);
        }

        XIMU3_Result getResult() {
            return XIMU3_network_announcement_get_result(wrapped);
        }

        uint64_t addCallback(std::function<void(XIMU3_NetworkAnnouncementMessage)> &callback) {
            return XIMU3_network_announcement_add_callback(wrapped, Helpers::wrapCallable<XIMU3_NetworkAnnouncementMessage>(callback), &callback);
        }

        void removeCallback(const uint64_t callbackId) {
            XIMU3_network_announcement_remove_callback(wrapped, callbackId);
        }

        std::vector<XIMU3_NetworkAnnouncementMessage> getMessages() {
            return toVectorAndFree(XIMU3_network_announcement_get_messages(wrapped));
        }

        std::vector<XIMU3_NetworkAnnouncementMessage> getMessagesAfterShortDelay() {
            return toVectorAndFree(XIMU3_network_announcement_get_messages_after_short_delay(wrapped));
        }

    private:
        XIMU3_NetworkAnnouncement *wrapped;

        static std::vector<XIMU3_NetworkAnnouncementMessage> toVectorAndFree(const XIMU3_NetworkAnnouncementMessages &messages) {
            const std::vector<XIMU3_NetworkAnnouncementMessage> vector = Helpers::toVector<XIMU3_NetworkAnnouncementMessage>(messages);
            XIMU3_network_announcement_messages_free(messages);
            return vector;
        }
    };
} // namespace ximu3
