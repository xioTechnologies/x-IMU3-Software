#pragma once

#include "../C/Ximu3.h"
#include <functional>
#include "Helpers.hpp"
#include <string>
#include <vector>

namespace ximu3 {
    class PortScanner {
    public:
        explicit PortScanner(std::function<void(const std::vector<XIMU3_Device> &)> callback_) {
            callback = [callback_](XIMU3_Devices devices) {
                callback_(toVectorAndFree(devices));
            };
            wrapped = XIMU3_port_scanner_new(Helpers::wrapCallable<XIMU3_Devices>(callback), &callback);
        }

        ~PortScanner() {
            XIMU3_port_scanner_free(wrapped);
        }

        std::vector<XIMU3_Device> getDevices() {
            return toVectorAndFree(XIMU3_port_scanner_get_devices(wrapped));
        }

        static std::vector<XIMU3_Device> scan() {
            return toVectorAndFree(XIMU3_port_scanner_scan());
        }

        static std::vector<XIMU3_Device> scanFilter(XIMU3_PortType portType) {
            return toVectorAndFree(XIMU3_port_scanner_scan_filter(portType));
        }

        static std::vector<std::string> getPortNames() {
            return Helpers::toVectorAndFree(XIMU3_port_scanner_get_port_names());
        }

    private:
        XIMU3_PortScanner *wrapped;
        std::function<void(XIMU3_Devices)> callback;

        static std::vector<XIMU3_Device> toVectorAndFree(const XIMU3_Devices &devices) {
            const std::vector<XIMU3_Device> vector = Helpers::toVector<XIMU3_Device>(devices);
            XIMU3_devices_free(devices);
            return vector;
        }
    };
} // namespace ximu3
