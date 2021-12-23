#pragma once

#include "../Widgets/DiscoveredDevicesTable.h"
#include "../Widgets/IconButton.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "SerialDiscovery.hpp"

class SearchForConnectionsDialog : public Dialog,
                                   private juce::ChangeListener
{
public:
    SearchForConnectionsDialog(std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections_);

    ~SearchForConnectionsDialog() override;

    void resized() override;

    std::vector<ximu3::ConnectionInfo*> getConnectionInfos() const;

private:
    const std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections;

    NetworkDiscoveryDispatcher& networkDiscoveryDispatcher = NetworkDiscoveryDispatcher::getSingleton();

    std::vector<ximu3::XIMU3_DiscoveredSerialDevice> serialDevices;

    ximu3::SerialDiscovery serialDiscovery {
            [this](auto devices_)
            {
                auto self = SafePointer<juce::Component>(this);
                juce::MessageManager::callAsync([this, self, devices_]
                                                {
                                                    if (self != nullptr)
                                                    {
                                                        serialDevices = devices_;
                                                        update();
                                                    }
                                                });
            }
    };

    DiscoveredDevicesTable table;

    IconButton filterButton { IconButton::Style::menuStripDropdown, BinaryData::filter_svg, 0.8f, "Filter", std::bind(&SearchForConnectionsDialog::getFilterMenu, this) };

    void update();

    juce::PopupMenu getFilterMenu();

    void changeListenerCallback(juce::ChangeBroadcaster*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SearchForConnectionsDialog)
};
