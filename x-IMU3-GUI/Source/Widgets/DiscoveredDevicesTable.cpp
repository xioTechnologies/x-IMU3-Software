#include "../CustomLookAndFeel.h"
#include "DiscoveredDevicesTable.h"
#include "Ximu3.hpp"

DiscoveredDevicesTable::DiscoveredDevicesTable(std::vector<DiscoveredDevice>& discoveredDevices_) : discoveredDevices(discoveredDevices_)
{
    addAndMakeVisible(numConnectionsFoundLabel);

    addAndMakeVisible(table);
    table.getHeader().addColumn("", selectionColumnID, 40, 40, 40);
    table.getHeader().addColumn("", nameAndSerialNumberColumnID, 240);
    table.getHeader().addColumn("", infoColumnID, 280);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.setRowHeight(30);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setWantsKeyboardFocus(false);

    addAndMakeVisible(buttonSelectAll);
    buttonSelectAll.setWantsKeyboardFocus(false);
    buttonSelectAll.onClick = [&]
    {
        for (size_t i = 0; i < discoveredDevices.size(); i++)
        {
            discoveredDevices[i].selected = buttonSelectAll.getToggleState();

            if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent(selectionColumnID, (int) i)))
            {
                toggle->setToggleState(discoveredDevices[i].selected, juce::dontSendNotification);
            }
        }

        selectionChanged();
    };

    addAndMakeVisible(labelSelectAll);
    labelSelectAll.toBehind(&buttonSelectAll);
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

void DiscoveredDevicesTable::update()
{
    if (buttonSelectAll.getToggleState())
    {
        for (auto& device : discoveredDevices)
        {
            device.selected = true;
        }
    }

    selectionChanged();

    table.updateContent();

    juce::String labelText = "Found: ";

    if (discoveredDevices.empty())
    {
        labelText += "0 Connections";
    }
    else
    {
        std::map<ximu3::XIMU3_ConnectionType, int> numberOfConnections;
        for (auto& device : discoveredDevices)
        {
            numberOfConnections[device.connectionType]++;
        }

        for (const auto& pair : numberOfConnections)
        {
            labelText += juce::String(pair.second) + " " + juce::String(XIMU3_connection_type_to_string(pair.first));
            if (&pair != &*numberOfConnections.rbegin())
            {
                labelText += ", ";
            }
        }
    }

    numConnectionsFoundLabel.setText(labelText);
}

void DiscoveredDevicesTable::selectionChanged()
{
    for (auto& device : discoveredDevices)
    {
        if (device.selected == false)
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
    return (int) discoveredDevices.size();
}

void DiscoveredDevicesTable::paintRowBackground(juce::Graphics& g, int rowNumber, int, int, bool)
{
    g.fillAll(rowNumber % 2 == 0 ? UIColours::menuStrip : UIColours::background);
}

juce::Component* DiscoveredDevicesTable::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    switch (columnID)
    {
        case selectionColumnID:
        {
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new CustomToggleButton("");
            }

            auto* toggle = static_cast<CustomToggleButton*>(existingComponentToUpdate);
            toggle->setWantsKeyboardFocus(false);
            toggle->setToggleState(discoveredDevices[(size_t) rowNumber].selected, juce::dontSendNotification);
            toggle->onClick = [this, rowNumber, toggle]
            {
                discoveredDevices[(size_t) rowNumber].selected = toggle->getToggleState();
                selectionChanged();
            };
            break;
        }

        case nameAndSerialNumberColumnID:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new SimpleLabel();
            }


            static_cast<SimpleLabel*>(existingComponentToUpdate)->setText(discoveredDevices[(size_t) rowNumber].deviceNameAndSerialNumber);
            break;

        case infoColumnID:
            if (existingComponentToUpdate == nullptr)
            {
                existingComponentToUpdate = new SimpleLabel();
            }

            static_cast<SimpleLabel*>(existingComponentToUpdate)->setText(discoveredDevices[(size_t) rowNumber].connectionInfo->toString());
            break;

        default:
            break;
    }

    return existingComponentToUpdate;
}

void DiscoveredDevicesTable::cellClicked(int rowNumber, int, const juce::MouseEvent&)
{
    if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent(selectionColumnID, rowNumber)))
    {
        toggle->setToggleState(!discoveredDevices[(size_t) rowNumber].selected, juce::sendNotificationSync);
    }
}
