#pragma once

#include "ApplicationSettings.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Ximu3.hpp"

class RecentConnections
{
public:
    RecentConnections();

    void update(const ximu3::ConnectionInfo& connectionInfo);

    void update(std::pair<std::uint8_t, std::uint8_t> muxChannels);

    std::vector<std::variant<std::unique_ptr<ximu3::ConnectionInfo>, std::pair<std::uint8_t, std::uint8_t>>> get() const;

private:
    juce::ValueTree recentConnections;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Recent Connections.xml");

    void update(juce::ValueTree newChild);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecentConnections)
};
