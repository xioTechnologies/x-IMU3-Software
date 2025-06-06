#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/ConnectionsTable.h"
#include "Widgets/IconButton.h"
#include "Ximu3.hpp"

class AvailableConnectionsDialog : public Dialog,
                                   private juce::Timer
{
public:
    struct ExistingConnection
    {
        juce::String descriptor;
        std::shared_ptr<ximu3::ConnectionInfo> connectionInfo;

        bool operator==(const ximu3::ConnectionInfo& connectionInfo_) const
        {
            return connectionInfo->toString() == connectionInfo_.toString();
        }
    };

    AvailableConnectionsDialog(std::vector<ExistingConnection> existingConnections_);

    void resized() override;

    std::vector<ximu3::ConnectionInfo*> getConnectionInfos() const;

private:
    const std::vector<ExistingConnection> existingConnections;

    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;

    ximu3::PortScanner portScanner {
        [](const auto&)
        {
        }
    };

    ConnectionsTable table;

    IconButton filterButton { BinaryData::filter_svg, "Filter", std::bind(&AvailableConnectionsDialog::getFilterMenu, this) };

    juce::PopupMenu getFilterMenu();

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AvailableConnectionsDialog)
};
