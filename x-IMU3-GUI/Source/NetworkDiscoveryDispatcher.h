#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Ximu3.hpp"

class NetworkDiscoveryDispatcher : private juce::DeletedAtShutdown,
                                   public juce::ChangeBroadcaster
{
public:
    static NetworkDiscoveryDispatcher& getSingleton()
    {
        static auto* singleton = new NetworkDiscoveryDispatcher();
        return *singleton;
    }

    const std::vector<ximu3::XIMU3_DiscoveredNetworkDevice>& getDevices() const
    {
        return devices;
    }

private:
    NetworkDiscoveryDispatcher()
    {
    }

    ximu3::NetworkDiscovery discovery {
            [this](auto devices_)
            {
                juce::MessageManager::callAsync([this, devices_]
                                                {
                                                    devices = devices_;
                                                    sendSynchronousChangeMessage();
                                                });
            }
    };

    std::vector<ximu3::XIMU3_DiscoveredNetworkDevice> devices;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NetworkDiscoveryDispatcher)
};
