#include "ConnectionsTable.h"
#include "CustomLookAndFeel.h"
#include "Widgets/Icon.h"

ConnectionsTable::ConnectionsTable()
{
    addAndMakeVisible(selectAllButton);
    addAndMakeVisible(selectAllLabel);
    addAndMakeVisible(table);
    addAndMakeVisible(noConnectionsFoundLabel);

    selectAllButton.setToggleState(true, juce::dontSendNotification);
    selectAllButton.onClick = [&]
    {
        for (size_t index = 0; index < rows.size(); index++)
        {
            if (rows[index].unavailable)
            {
                continue;
            }

            if (auto* const toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent((int) ColumnIds::selected, (int) index)))
            {
                toggle->setToggleState(selectAllButton.getToggleState(), juce::dontSendNotification);
            }
            rows[index].selected = selectAllButton.getToggleState();
        }
    };

    table.getHeader().addColumn("", (int) ColumnIds::selected, 40, 40, 40);
    table.getHeader().addColumn("", (int) ColumnIds::descriptor, 1);
    table.getHeader().addColumn("", (int) ColumnIds::connection, 140);
    table.getHeader().addColumn("", (int) ColumnIds::rssi, 25, 25, 25);
    table.getHeader().addColumn("", (int) ColumnIds::battery, 25, 25, 25);
    table.getHeader().addColumn("", (int) ColumnIds::margin, 5, 5, 5);
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
    selectAllLabel.setBounds(selectAllButton.getBounds().withLeft(table.getHeader().getColumnPosition(((int) ColumnIds::descriptor - 1)).getX()));
    table.setBounds(bounds);
    noConnectionsFoundLabel.setBounds(bounds);
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

    std::stable_partition(rows.rbegin(), rows.rend(), [](const auto& row)
    {
        return row.unavailable;
    });

    for (auto& row : rows)
    {
        row.selected = row.selected || selectAllButton.getToggleState();
    }

    table.updateContent();

    noConnectionsFoundLabel.setVisible(rows.empty());
}

const std::vector<ConnectionsTable::Row>& ConnectionsTable::getRows() const
{
    return rows;
}

int ConnectionsTable::getNumRows()
{
    return (int) rows.size();
}

juce::Component* ConnectionsTable::refreshComponentForCell(int rowNumber, int columnId, bool, juce::Component* existingComponentToUpdate)
{
    if (rowNumber >= (int) rows.size())
    {
        return existingComponentToUpdate; // index may exceed size on Windows if display scaling >100%
    }

    switch ((ColumnIds) columnId)
    {
        case ColumnIds::selected:
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

        case ColumnIds::descriptor:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new SimpleLabel();
            }

            static_cast<SimpleLabel*>(existingComponentToUpdate)->setText(rows[(size_t) rowNumber].descriptor);
            break;

        case ColumnIds::connection:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new SimpleLabel();
            }

            static_cast<SimpleLabel*>(existingComponentToUpdate)->setText(rows[(size_t) rowNumber].connectionInfo->toString());
            break;

        case ColumnIds::rssi:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new RssiIcon(0.65f);
            }

            if (const auto percentage = rows[(size_t) rowNumber].rssiPercentage; percentage.has_value() && (percentage != -1))
            {
                static_cast<RssiIcon*>(existingComponentToUpdate)->update(*percentage);
            }
            else
            {
                static_cast<RssiIcon*>(existingComponentToUpdate)->unavailable();
            }
            break;

        case ColumnIds::battery:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new BatteryIcon(0.65f);
            }

            if (const auto percentage = rows[(size_t) rowNumber].batteryPercentage; percentage.has_value() && (percentage != -1))
            {
                static_cast<BatteryIcon*>(existingComponentToUpdate)->update(*percentage, *rows[(size_t) rowNumber].batteryStatus);
            }
            else
            {
                static_cast<BatteryIcon*>(existingComponentToUpdate)->unavailable();
            }
            break;

        case ColumnIds::margin:
            break;
    }

    if (existingComponentToUpdate != nullptr)
    {
        existingComponentToUpdate->setEnabled(rows[(size_t) rowNumber].unavailable == false);
    }
    return existingComponentToUpdate;
}

void ConnectionsTable::cellClicked(int rowNumber, int, const juce::MouseEvent&)
{
    if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent((int) ColumnIds::selected, rowNumber)))
    {
        if (rows[(size_t) rowNumber].unavailable == false)
        {
            toggle->setToggleState(!rows[(size_t) rowNumber].selected, juce::sendNotificationSync);
        }
    }
}
