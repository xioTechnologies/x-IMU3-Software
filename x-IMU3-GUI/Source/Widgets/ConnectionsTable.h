#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomToggleButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class ConnectionsTable : public juce::Component,
                         private juce::TableListBoxModel
{
public:
    struct Row
    {
        bool selected;
        juce::String deviceName;
        juce::String serialNumber;
        std::shared_ptr<ximu3::ConnectionInfo> connectionInfo;
        ximu3::XIMU3_ConnectionType connectionType;
        std::optional<int> rssiPercentage;
        std::optional<int> batteryPercentage;
        std::optional<ximu3::XIMU3_ChargingStatus> batteryStatus;

        bool operator==(const Row& row) const
        {
            return connectionInfo->toString() == row.connectionInfo->toString();
        }
    };

    ConnectionsTable();

    void resized() override;

    void setRows(std::vector<Row> rows_);

    const std::vector<Row>& getRows() const;

private:
    enum class ColumnIDs
    {
        selected = 1,
        device,
        connection,
        rssi,
        battery,
        margin,
    };

    std::vector<Row> rows;

    CustomToggleButton selectAllButton { "" };
    SimpleLabel selectAllLabel { "Select All" };
    juce::TableListBox table { "", this };

    SimpleLabel noConnectionsFoundLabel { "No Connections Found", UIFonts::getDefaultFont(), juce::Justification::centred };

    int getNumRows() override;

    void paintRowBackground(juce::Graphics&, int, int, int, bool) override
    {
    }

    void paintCell(juce::Graphics&, int, int, int, int, bool) override
    {
    }

    juce::Component* refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate) override;

    void cellClicked(int rowNumber, int, const juce::MouseEvent&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionsTable)
};
