#pragma once

#include "../Widgets/ConnectionsTable.h"
#include "../Widgets/IconButton.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Ximu3.hpp"

class SearchForConnectionsDialog : public Dialog,
                                   private juce::Timer
{
public:
    SearchForConnectionsDialog(std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections_);

    void resized() override;

    std::vector<ximu3::ConnectionInfo*> getConnectionInfos() const;

private:
    const std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections;

    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;

    ximu3::PortScanner portScanner { [](const auto&)
                                     {
                                     }};

    ConnectionsTable table;

    IconButton filterButton { BinaryData::filter_svg, "Filter", std::bind(&SearchForConnectionsDialog::getFilterMenu, this) };

    juce::PopupMenu getFilterMenu();

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SearchForConnectionsDialog)
};
