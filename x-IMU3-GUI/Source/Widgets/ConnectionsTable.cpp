#include "../CustomLookAndFeel.h"
#include "ConnectionsTable.h"

ConnectionsTable::ConnectionsTable()
{
    addAndMakeVisible(selectAllButton);
    addAndMakeVisible(deviceLabel);
    addAndMakeVisible(connectionLabel);
    addAndMakeVisible(table);

    selectAllButton.setToggleState(true, juce::dontSendNotification);
    selectAllButton.onClick = [&]
    {
        for (size_t index = 0; index < rows.size(); index++)
        {
            if (auto* const toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent((int) ColumnIDs::selected, (int) index)))
            {
                toggle->setToggleState(selectAllButton.getToggleState(), juce::dontSendNotification);
            }
            rows[index].selected = selectAllButton.getToggleState();
        }
    };

    table.getHeader().addColumn("", (int) ColumnIDs::selected, 40, 40, 40);
    table.getHeader().addColumn("", (int) ColumnIDs::device, 1);
    table.getHeader().addColumn("", (int) ColumnIDs::connection, 140);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setWantsKeyboardFocus(false);
}

void ConnectionsTable::resized()
{
    static constexpr int headerHeight = 30;

    auto bounds = getLocalBounds();
    selectAllButton.setBounds(bounds.removeFromTop(headerHeight));
    deviceLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::device - 1).withHeight(headerHeight));
    connectionLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::connection - 1).withHeight(headerHeight));
    table.setBounds(bounds);
}

void ConnectionsTable::setRows(std::vector<Row> rows_)
{
    static const auto find = [](auto& vector, const auto& value)
    {
        auto it = std::find(vector.begin(), vector.end(), value);
        return it != vector.end() ? &*it : nullptr;
    };

    for (auto& previousRow : std::vector<Row>(std::move(rows)))
    {
        if (auto* const newRow = find(rows_, previousRow))
        {
            newRow->selected = previousRow.selected;
            rows.push_back(*newRow);
        }
    }

    for (auto& row : rows_)
    {
        if (find(rows, row) == nullptr)
        {
            rows.push_back(std::move(row));
        }
    }

    for (auto& row : rows)
    {
        row.selected |= selectAllButton.getToggleState();
    }

    table.updateContent();
}

const std::vector<ConnectionsTable::Row>& ConnectionsTable::getRows() const
{
    return rows;
}

int ConnectionsTable::getNumRows()
{
    return (int) rows.size();
}

juce::Component* ConnectionsTable::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    if (rowNumber >= (int) rows.size())
    {
        return existingComponentToUpdate; // index may exceed size on Windows if display scaling >100%
    }

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
                selectAllButton.setToggleState(selectAllButton.getToggleState() && toggle->getToggleState(), juce::dontSendNotification);
            };
            break;
        }

        case ColumnIDs::device:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new SimpleLabel();
            }

            static_cast<SimpleLabel*>(existingComponentToUpdate)->setText(rows[(size_t) rowNumber].deviceName + " " + rows[(size_t) rowNumber].serialNumber);
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

void ConnectionsTable::cellClicked(int rowNumber, int, const juce::MouseEvent&)
{
    if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent((int) ColumnIDs::selected, rowNumber)))
    {
        toggle->setToggleState(!rows[(size_t) rowNumber].selected, juce::sendNotificationSync);
    }
}
