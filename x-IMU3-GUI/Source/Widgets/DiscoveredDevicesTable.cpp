#include "../CustomLookAndFeel.h"
#include "DiscoveredDevicesTable.h"
#include "Ximu3.hpp"

DiscoveredDevicesTable::DiscoveredDevicesTable()
{
    addAndMakeVisible(numConnectionsFoundLabel);

    addAndMakeVisible(table);
    table.getHeader().addColumn("", selectedColumnID, 40, 40, 40);
    table.getHeader().addColumn("", nameAndSerialNumberColumnID, 240);
    table.getHeader().addColumn("", connectionInfoColumnID, 280);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.setRowHeight(30);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setWantsKeyboardFocus(false);

    addAndMakeVisible(buttonSelectAll);
    buttonSelectAll.onClick = [&]
    {
        for (size_t index = 0; index < rows.size(); index++)
        {
            rows[index].selected = buttonSelectAll.getToggleState();

            if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent(selectedColumnID, (int) index)))
            {
                toggle->setToggleState(rows[index].selected, juce::dontSendNotification);
            }
        }

        selectionChanged();
    };

    addAndMakeVisible(labelSelectAll);
    labelSelectAll.toBehind(&buttonSelectAll);
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

    if (buttonSelectAll.getToggleState())
    {
        for (auto& row : rows)
        {
            row.selected = true;
        }
    }

    selectionChanged();

    table.updateContent();

    juce::String text = "Found: ";

    if (rows.empty())
    {
        text += "0 Connections";
    }
    else
    {
        std::map<ximu3::XIMU3_ConnectionType, int> numberOfConnections;
        for (auto& row : rows)
        {
            numberOfConnections[row.connectionType]++;
        }

        for (const auto& pair : numberOfConnections)
        {
            text += juce::String(pair.second) + " " + juce::String(XIMU3_connection_type_to_string(pair.first));
            if (&pair != &*numberOfConnections.rbegin())
            {
                text += ", ";
            }
        }
    }

    numConnectionsFoundLabel.setText(text);
}

const std::vector<DiscoveredDevicesTable::Row>& DiscoveredDevicesTable::getRows() const
{
    return rows;
}

void DiscoveredDevicesTable::paint(juce::Graphics& g)
{
    g.setColour(UIColours::menuStrip);
    g.fillRect(numConnectionsFoundLabel.getBounds().withSizeKeepingCentre(numConnectionsFoundLabel.getWidth(), table.getRowHeight()));
    g.setColour(UIColours::background);
    g.fillRect(buttonSelectAll.getBounds().getUnion(labelSelectAll.getBounds()));
}

void DiscoveredDevicesTable::resized()
{
    auto bounds = getLocalBounds();

    auto foundBounds = bounds.removeFromTop(UILayout::textComponentHeight);
    foundBounds.removeFromLeft(6); // align with toggle checkbox, from CustomToggleButtonLookAndFeel
    numConnectionsFoundLabel.setBounds(foundBounds);

    auto selectAllBounds = bounds.removeFromTop(table.getRowHeight());
    buttonSelectAll.setBounds(selectAllBounds);
    selectAllBounds.removeFromLeft(table.getHeader().getColumnPosition(1).getX());
    labelSelectAll.setBounds(selectAllBounds);

    table.setBounds(bounds);
}

void DiscoveredDevicesTable::selectionChanged()
{
    for (auto& row : rows)
    {
        if (row.selected == false)
        {
            buttonSelectAll.setToggleState(false, juce::dontSendNotification);
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

void DiscoveredDevicesTable::paintRowBackground(juce::Graphics& g, int rowNumber, int, int, bool)
{
    g.fillAll(rowNumber % 2 == 0 ? UIColours::menuStrip : UIColours::background);
}

juce::Component* DiscoveredDevicesTable::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    switch (columnID)
    {
        case selectedColumnID:
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

        case nameAndSerialNumberColumnID:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new SimpleLabel();
            }

            static_cast<SimpleLabel*>(existingComponentToUpdate)->setText(rows[(size_t) rowNumber].deviceNameAndSerialNumber);
            break;

        case connectionInfoColumnID:
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
    if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent(selectedColumnID, rowNumber)))
    {
        toggle->setToggleState(!rows[(size_t) rowNumber].selected, juce::sendNotificationSync);
    }
}
