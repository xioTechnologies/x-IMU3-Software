#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ConnectionsRow.h"
#include "CustomToggleButton.h"
#include "SimpleLabel.h"

class ConnectionsTable : public juce::Component {
public:
    ConnectionsTable();

    void paint(juce::Graphics &g) override;

    void resized() override;

    void setRows(std::vector<ConnectionsRow::Data> rows_);

    std::vector<ximu3::ConnectionConfig *> getConnectionConfigs() const;

private:
    static constexpr int rowHeight = 22;

    CustomToggleButton selectAllButton{""};
    SimpleLabel selectAllLabel{"Select All"};
    juce::Viewport viewport;
    juce::Component content;
    std::vector<std::unique_ptr<ConnectionsRow>> rows;

    SimpleLabel noConnectionsFoundLabel{"No Connections Found", UIFonts::getDefaultFont(), juce::Justification::centred};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionsTable)
};
