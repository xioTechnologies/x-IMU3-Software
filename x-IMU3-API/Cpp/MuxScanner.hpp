#pragma once

#include "../C/Ximu3.h"
#include "Connection.hpp"
#include <functional>
#include "Helpers.hpp"
#include <vector>

namespace ximu3 {
    class MuxScanner {
    public:
        explicit MuxScanner(const Connection &connection) {
            wrapped = XIMU3_mux_scanner_new(connection.wrapped);
        }

        ~MuxScanner() {
            XIMU3_mux_scanner_free(wrapped);
        }

        uint64_t addCallback(std::function<void(const std::vector<ximu3::Device> &)> &callback) {
            const auto internalCallback = +[](XIMU3_Devices devices, void *context) {
                (*static_cast<decltype(&callback)>(context))(Helpers::toVectorAndFree(devices));
            };
            return XIMU3_mux_scanner_add_callback(wrapped, internalCallback, &callback);
        }

        void removeCallback(const uint64_t id) {
            XIMU3_mux_scanner_remove_callback(wrapped, id);
        }

        std::vector<ximu3::Device> getDevices() {
            return Helpers::toVectorAndFree(XIMU3_mux_scanner_get_devices(wrapped));
        }

        static std::vector<ximu3::Device> scan(const Connection &connection, const uint32_t numberOfChannels = XIMU3_MAX_NUMBER_OF_MUX_CHANNELS, const uint32_t retries = XIMU3_DEFAULT_RETRIES, const uint32_t timeout = XIMU3_DEFAULT_TIMEOUT) {
            return Helpers::toVectorAndFree(XIMU3_mux_scanner_scan(connection.wrapped, numberOfChannels, retries, timeout));
        }

    private:
        XIMU3_MuxScanner *wrapped;
    };
} // namespace ximu3
