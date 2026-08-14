#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/ConnectionsTable.h"
#include "Widgets/IconButton.h"
#include "Ximu3.hpp"

class FindConnectionsDialog : public Dialog,
                              private juce::Timer {
public:
    FindConnectionsDialog(std::vector<std::shared_ptr<ximu3::Connection> > existingConnections_);

    void resized() override;

    std::vector<ximu3::ConnectionConfig *> getConnectionConfigs() const;

private:
    const std::vector<std::shared_ptr<ximu3::Connection> > existingConnections;

    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;

    ximu3::PortScanner portScanner;

    std::vector<std::unique_ptr<ximu3::MuxScanner> > muxScanners;

    ConnectionsTable table;

    IconButton filterButton{BinaryData::checklist_svg, "Filter", std::bind(&FindConnectionsDialog::getFilterMenu, this)};

    juce::PopupMenu getFilterMenu();

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FindConnectionsDialog)
};
