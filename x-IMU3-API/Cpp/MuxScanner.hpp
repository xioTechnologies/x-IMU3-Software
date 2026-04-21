#pragma once

#include "../C/Ximu3.h"
#include "Connection.hpp"
#include <functional>
#include "Helpers.hpp"
#include <vector>

namespace ximu3 {
    class MuxScanner {
    public:
        explicit MuxScanner(const Connection &connection, std::function<void(const std::vector<XIMU3_Device> &)> callback_) {
            callback = [callback_](XIMU3_Devices devices) {
                callback_(Helpers::toVectorAndFree(devices));
            };
            wrapped = XIMU3_mux_scanner_new(connection.wrapped, Helpers::wrapCallable<XIMU3_Devices>(callback), &callback);
        }

        ~MuxScanner() {
            XIMU3_mux_scanner_free(wrapped);
        }

        std::vector<XIMU3_Device> getDevices() {
            return Helpers::toVectorAndFree(XIMU3_mux_scanner_get_devices(wrapped));
        }

        static std::vector<XIMU3_Device> scan(const Connection &connection, const uint32_t numberOfChannels = XIMU3_MAX_NUMBER_OF_MUX_CHANNELS, const uint32_t retries = XIMU3_DEFAULT_RETRIES, const uint32_t timeout = XIMU3_DEFAULT_TIMEOUT) {
            return Helpers::toVectorAndFree(XIMU3_mux_scanner_scan(connection.wrapped, numberOfChannels, retries, timeout));
        }

    private:
        XIMU3_MuxScanner *wrapped;
        std::function<void(XIMU3_Devices)> callback;
    };
} // namespace ximu3
