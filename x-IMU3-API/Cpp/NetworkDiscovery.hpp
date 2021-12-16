#pragma once

#include "../C/Ximu3.h"
#include <functional>
#include "Helpers.hpp"
#include <string>
#include <vector>

namespace ximu3
{
    class NetworkDiscovery
    {
    public:
        explicit NetworkDiscovery(std::function<void(const std::vector<XIMU3_DiscoveredNetworkDevice>&)> callback)
        {
            internalCallback = [callback](XIMU3_DiscoveredNetworkDevices devices)
            {
                callback(toVectorAndFree(devices));
            };
            discovery = XIMU3_network_discovery_new(Helpers::wrapCallable<XIMU3_DiscoveredNetworkDevices>(internalCallback), &internalCallback);
        }

        ~NetworkDiscovery()
        {
            XIMU3_network_discovery_free(discovery);
        }

        std::vector<XIMU3_DiscoveredNetworkDevice> getDevices()
        {
            return toVectorAndFree(XIMU3_network_discovery_get_devices(discovery));
        }

        static std::vector<XIMU3_DiscoveredNetworkDevice> scan(const uint32_t milliseconds)
        {
            return toVectorAndFree(XIMU3_network_discovery_scan(milliseconds));
        }

    private:
        XIMU3_NetworkDiscovery* discovery;
        std::function<void(XIMU3_DiscoveredNetworkDevices)> internalCallback;

        static std::vector<XIMU3_DiscoveredNetworkDevice> toVectorAndFree(const XIMU3_DiscoveredNetworkDevices& devices)
        {
            const std::vector<XIMU3_DiscoveredNetworkDevice> vector = Helpers::toVector<XIMU3_DiscoveredNetworkDevice>(devices);
            XIMU3_discovered_network_devices_free(devices);
            return vector;
        }
    };
} // namespace ximu3
