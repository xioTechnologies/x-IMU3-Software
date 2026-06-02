#pragma once

#include "../C/Ximu3.h"
#include <functional>
#include "Helpers.hpp"
#include <string>
#include <vector>

namespace ximu3 {
    class PortScanner {
    public:
        PortScanner() {
            wrapped = XIMU3_port_scanner_new();
        }

        ~PortScanner() {
            XIMU3_port_scanner_free(wrapped);
        }

        uint64_t addCallback(std::function<void(const std::vector<ximu3::Device> &)> &callback) {
            const auto internalCallback = +[](XIMU3_Devices devices, void *context) {
                (*static_cast<decltype(&callback)>(context))(Helpers::toVectorAndFree(devices));
            };
            return XIMU3_port_scanner_add_callback(wrapped, internalCallback, &callback);
        }

        void removeCallback(const uint64_t id) {
            XIMU3_port_scanner_remove_callback(wrapped, id);
        }

        std::vector<ximu3::Device> getDevices() {
            return Helpers::toVectorAndFree(XIMU3_port_scanner_get_devices(wrapped));
        }

        static std::vector<ximu3::Device> scan() {
            return Helpers::toVectorAndFree(XIMU3_port_scanner_scan());
        }

        static std::vector<ximu3::Device> scanFilter(XIMU3_PortType portType) {
            return Helpers::toVectorAndFree(XIMU3_port_scanner_scan_filter(portType));
        }

        static std::vector<std::string> getPortNames() {
            return toVectorAndFree(XIMU3_port_scanner_get_port_names());
        }

    private:
        XIMU3_PortScanner *wrapped;

        static std::vector<std::string> toVectorAndFree(const XIMU3_CharArrays &charArrays) {
            const auto vector = Helpers::toVector<std::string>(charArrays);
            XIMU3_char_arrays_free(charArrays);
            return vector;
        }
    };
} // namespace ximu3
