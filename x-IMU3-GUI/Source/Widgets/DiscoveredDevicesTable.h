#pragma once

#include "../Widgets/CustomToggleButton.h"
#include "../Widgets/SimpleLabel.h"
#include "ConnectionInfo.hpp"
#include <juce_gui_basics/juce_gui_basics.h>
#include <variant>
#include "Ximu3.hpp"

class DiscoveredDevicesTable : public juce::Component,
                               private juce::TableListBoxModel
{
public:
    struct Row
    {
        bool selected = false;
        juce::String deviceNameAndSerialNumber;
        std::unique_ptr<ximu3::ConnectionInfo> connectionInfo;
        ximu3::XIMU3_ConnectionType connectionType;

        bool operator==(const Row& row) const
        {
            return connectionInfo->toString() == row.connectionInfo->toString();
        }
    };

    DiscoveredDevicesTable();

    void resized() override;

    void setRows(std::vector<Row> rows_);

    const std::vector<Row>& getRows() const;

    std::function<void()> onSelectionChanged;

private:
    enum class ColumnIDs
    {
        selected = 1,
        device,
        connection,
    };

    std::vector<Row> rows;

    CustomToggleButton selectAllButton { "" };
    SimpleLabel deviceLabel { "Device" };
    SimpleLabel connectionLabel { "Connection" };
    juce::TableListBox table { "", this };

    void selectionChanged();

    int getNumRows() override;

    void paintRowBackground(juce::Graphics&, int, int, int, bool) override
    {
    }

    void paintCell(juce::Graphics&, int, int, int, int, bool) override
    {
    }

    juce::Component* refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate) override;

    void cellClicked(int rowNumber, int, const juce::MouseEvent&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiscoveredDevicesTable)
};
