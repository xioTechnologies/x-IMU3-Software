#pragma once

#include "../C/Ximu3.h"
#include <functional>
#include "Helpers.hpp"
#include <string>
#include <vector>

namespace ximu3
{
    class SerialDiscovery
    {
    public:
        explicit SerialDiscovery(std::function<void(const std::vector<XIMU3_DiscoveredSerialDevice>&)> callback)
        {
            internalCallback = [callback](XIMU3_DiscoveredSerialDevices devices)
            {
                callback(toVectorAndFree(devices));
            };
            discovery = XIMU3_serial_discovery_new(Helpers::wrapCallable<XIMU3_DiscoveredSerialDevices>(internalCallback), &internalCallback);
        }

        ~SerialDiscovery()
        {
            XIMU3_serial_discovery_free(discovery);
        }

        std::vector<XIMU3_DiscoveredSerialDevice> getDevices()
        {
            return toVectorAndFree(XIMU3_serial_discovery_get_devices(discovery));
        }

        static std::vector<XIMU3_DiscoveredSerialDevice> scan(const uint32_t milliseconds)
        {
            return toVectorAndFree(XIMU3_serial_discovery_scan(milliseconds));
        }

        static std::vector<XIMU3_DiscoveredSerialDevice> scanFilter(const uint32_t milliseconds, XIMU3_ConnectionType connectionType)
        {
            return toVectorAndFree(XIMU3_serial_discovery_scan_filter(milliseconds, connectionType));
        }

        static std::vector<std::string> getAvailablePorts()
        {
            return Helpers::toVectorAndFree(XIMU3_serial_discovery_get_available_ports());
        }

    private:
        XIMU3_SerialDiscovery* discovery;
        std::function<void(XIMU3_DiscoveredSerialDevices)> internalCallback;

        static std::vector<XIMU3_DiscoveredSerialDevice> toVectorAndFree(const XIMU3_DiscoveredSerialDevices& devices)
        {
            const std::vector<XIMU3_DiscoveredSerialDevice> vector = Helpers::toVector<XIMU3_DiscoveredSerialDevice>(devices);
            XIMU3_discovered_serial_devices_free(devices);
            return vector;
        }
    };
} // namespace ximu3
