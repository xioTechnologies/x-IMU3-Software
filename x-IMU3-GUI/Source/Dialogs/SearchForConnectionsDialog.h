#pragma once

#include "../Widgets/DiscoveredDevicesTable.h"
#include "../Widgets/IconButton.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "SerialDiscovery.hpp"

class SearchForConnectionsDialog : public Dialog,
                                   private juce::ChangeListener,
                                   private juce::AsyncUpdater
{
public:
    SearchForConnectionsDialog();

    ~SearchForConnectionsDialog() override;

    void resized() override;

    std::vector<std::unique_ptr<ximu3::ConnectionInfo>> getConnectionInfos() const;

private:
    NetworkDiscoveryDispatcher& networkDiscoveryDispatcher = NetworkDiscoveryDispatcher::getSingleton();

    mutable std::mutex devicesMutex;
    std::vector<ximu3::XIMU3_DiscoveredSerialDevice> devices;

    ximu3::SerialDiscovery serialDiscovery {
            [this](auto devices_)
            {
                {
                    std::lock_guard<std::mutex> lock(devicesMutex);
                    devices = devices_;
                }

                triggerAsyncUpdate();
            }
    };

    DiscoveredDevicesTable table;

    IconButton filterButton { IconButton::Style::menuStripDropdown, BinaryData::filter_svg, 0.8f, "Filter", std::bind(&SearchForConnectionsDialog::getFilterMenu, this) };

    void updateTable();

    juce::PopupMenu getFilterMenu();

    void changeListenerCallback(juce::ChangeBroadcaster*) override;

    void handleAsyncUpdate() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SearchForConnectionsDialog)
};
