#include "../CustomLookAndFeel.h"
#include "DiscoveredDevicesTable.h"
#include "Ximu3.hpp"

DiscoveredDevicesTable::DiscoveredDevicesTable()
{
    addAndMakeVisible(selectAllButton);
    selectAllButton.onClick = [&]
    {
        for (size_t index = 0; index < rows.size(); index++)
        {
            rows[index].selected = selectAllButton.getToggleState();

            if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent((int) ColumnIDs::selected, (int) index)))
            {
                toggle->setToggleState(rows[index].selected, juce::dontSendNotification);
            }
        }

        selectionChanged();
    };

    addAndMakeVisible(deviceLabel);
    addAndMakeVisible(connectionLabel);

    addAndMakeVisible(table);
    table.getHeader().addColumn("", (int) ColumnIDs::selected, 40, 40, 40);
    table.getHeader().addColumn("", (int) ColumnIDs::device, 240);
    table.getHeader().addColumn("", (int) ColumnIDs::connection, 280);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setColour(juce::TableListBox::backgroundColourId, UIColours::background);
    table.setWantsKeyboardFocus(false);
}

void DiscoveredDevicesTable::resized()
{
    auto bounds = getLocalBounds();

    static constexpr int headerHeight = 30;
    bounds.removeFromTop(headerHeight);
    selectAllButton.setBounds(0, 0, getWidth(), 30);
    deviceLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::device - 1).withHeight(headerHeight));
    connectionLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::connection - 1).withHeight(headerHeight));

    table.setBounds(bounds);
}

void DiscoveredDevicesTable::setRows(std::vector<Row> rows_)
{
    static const auto contains = [](const auto& vector, const auto& value)
    {
        return std::find(vector.begin(), vector.end(), value) != vector.end();
    };

    for (auto& previousRow : std::vector<Row>(std::move(rows)))
    {
        if (contains(rows_, previousRow))
        {
            rows.push_back(std::move(previousRow));
        }
    }

    for (auto& row : rows_)
    {
        if (contains(rows, row) == false)
        {
            rows.push_back(std::move(row));
        }
    }

    if (selectAllButton.getToggleState())
    {
        for (auto& row : rows)
        {
            row.selected = true;
        }
    }

    selectionChanged();

    table.updateContent();
}

const std::vector<DiscoveredDevicesTable::Row>& DiscoveredDevicesTable::getRows() const
{
    return rows;
}

void DiscoveredDevicesTable::selectionChanged()
{
    for (auto& row : rows)
    {
        if (row.selected == false)
        {
            selectAllButton.setToggleState(false, juce::dontSendNotification);
            break;
        }
    }

    if (onSelectionChanged)
    {
        onSelectionChanged();
    }
}

int DiscoveredDevicesTable::getNumRows()
{
    return (int) rows.size();
}

juce::Component* DiscoveredDevicesTable::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    switch ((ColumnIDs) columnID)
    {
        case ColumnIDs::selected:
        {
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new CustomToggleButton("");
            }

            auto* toggle = static_cast<CustomToggleButton*>(existingComponentToUpdate);
            toggle->setToggleState(rows[(size_t) rowNumber].selected, juce::dontSendNotification);
            toggle->onClick = [this, rowNumber, toggle]
            {
                rows[(size_t) rowNumber].selected = toggle->getToggleState();
                selectionChanged();
            };
            break;
        }

        case ColumnIDs::device:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new SimpleLabel();
            }

            static_cast<SimpleLabel*>(existingComponentToUpdate)->setText(rows[(size_t) rowNumber].deviceNameAndSerialNumber);
            break;

        case ColumnIDs::connection:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new SimpleLabel();
            }

            static_cast<SimpleLabel*>(existingComponentToUpdate)->setText(rows[(size_t) rowNumber].connectionInfo->toString());
            break;

        default:
            break;
    }

    return existingComponentToUpdate;
}

void DiscoveredDevicesTable::cellClicked(int rowNumber, int, const juce::MouseEvent&)
{
    if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent((int) ColumnIDs::selected, rowNumber)))
    {
        toggle->setToggleState(!rows[(size_t) rowNumber].selected, juce::sendNotificationSync);
    }
}
