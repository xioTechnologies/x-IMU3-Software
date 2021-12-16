#include "../CustomLookAndFeel.h"
#include "DiscoveredDevicesTable.h"
#include "Ximu3.hpp"

juce::String DiscoveredDevicesTable::TableRowModel::getNameAndSerialNumber() const
{
    const juce::String separator = " - ";
    try
    {
        return std::get<ximu3::XIMU3_DiscoveredNetworkDevice>(device).device_name + separator + std::get<ximu3::XIMU3_DiscoveredNetworkDevice>(device).serial_number;
    }
    catch (const std::bad_variant_access&)
    {
        return std::get<ximu3::XIMU3_DiscoveredSerialDevice>(device).device_name + separator + std::get<ximu3::XIMU3_DiscoveredSerialDevice>(device).serial_number;
    }
}

std::unique_ptr<ximu3::ConnectionInfo> DiscoveredDevicesTable::TableRowModel::createConnectionInfo() const
{
    switch (connectionType)
    {
        case ximu3::XIMU3_ConnectionTypeUdp:
        {
            const auto& connectionInfo = std::get<ximu3::XIMU3_DiscoveredNetworkDevice>(device).udp_connection_info;
            return std::make_unique<ximu3::UdpConnectionInfo>(connectionInfo.ip_address, connectionInfo.send_port, connectionInfo.receive_port);
        }
        case ximu3::XIMU3_ConnectionTypeTcp:
        {
            const auto& connectionInfo = std::get<ximu3::XIMU3_DiscoveredNetworkDevice>(device).tcp_connection_info;
            return std::make_unique<ximu3::TcpConnectionInfo>(connectionInfo.ip_address, connectionInfo.port);
        }
        case ximu3::XIMU3_ConnectionTypeUsb:
        {
            const auto& connectionInfo = std::get<ximu3::XIMU3_DiscoveredSerialDevice>(device).usb_connection_info;
            return std::make_unique<ximu3::UsbConnectionInfo>(connectionInfo.port_name);
        }
        case ximu3::XIMU3_ConnectionTypeSerial:
        {
            const auto& connectionInfo = std::get<ximu3::XIMU3_DiscoveredSerialDevice>(device).serial_connection_info;
            return std::make_unique<ximu3::SerialConnectionInfo>(connectionInfo.port_name, connectionInfo.baud_rate, connectionInfo.rts_cts_enabled);
        }
        case ximu3::XIMU3_ConnectionTypeBluetooth:
        {
            const auto& connectionInfo = std::get<ximu3::XIMU3_DiscoveredSerialDevice>(device).bluetooth_connection_info;
            return std::make_unique<ximu3::BluetoothConnectionInfo>(connectionInfo.port_name);
        }
        case ximu3::XIMU3_ConnectionTypeFile:
        {
            return nullptr;
        }
    }

    return nullptr; // avoid warning C4715: "not all control paths return a value"
}

DiscoveredDevicesTable::DiscoveredDevicesTable()
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
        for (size_t i = 0; i < rows.size(); i++)
        {
            rows[i].selected = buttonSelectAll.getToggleState();

            if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent(selectionColumnID, (int) i)))
            {
                toggle->setToggleState(rows[i].selected, juce::dontSendNotification);
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

void DiscoveredDevicesTable::updateDiscoveredDevices(const std::vector<ximu3::XIMU3_DiscoveredSerialDevice>& discoveredSerialDevices,
                                                     const std::vector<ximu3::XIMU3_DiscoveredNetworkDevice>& discoveredNetworkDevices,
                                                     const Filter filter)
{
    std::vector<TableRowModel> newRows;

    for (auto& device : discoveredSerialDevices)
    {
        if (((device.connection_type == ximu3::XIMU3_ConnectionTypeUsb) && filter.usb) ||
            ((device.connection_type == ximu3::XIMU3_ConnectionTypeSerial) && filter.serial) ||
            ((device.connection_type == ximu3::XIMU3_ConnectionTypeBluetooth) && filter.bluetooth))
        {
            TableRowModel item;
            item.device = device;
            item.connectionType = device.connection_type;
            newRows.push_back(item);
        }
    }

    for (auto& device : discoveredNetworkDevices)
    {
        TableRowModel item;
        item.device = device;

        if (filter.udp)
        {
            item.connectionType = ximu3::XIMU3_ConnectionTypeUdp;
            newRows.push_back(item);
        }
        if (filter.tcp)
        {
            item.connectionType = ximu3::XIMU3_ConnectionTypeTcp;
            newRows.push_back(item);
        }
    }

    for (auto& newRow : newRows)
    {
        if (buttonSelectAll.getToggleState())
        {
            newRow.selected = true;
        }
        else
        {
            for (auto& row : rows)
            {
                if (row.createConnectionInfo()->toString() == newRow.createConnectionInfo()->toString())
                {
                    newRow.selected = row.selected;
                }
            }
        }
    }

    rows = newRows;
    selectionChanged();

    table.updateContent();

    std::map<ximu3::XIMU3_ConnectionType, int> numberOfConnectionsPerType;

    for (size_t rowIndex = 0; rowIndex < rows.size(); rowIndex++)
    {
        if (auto* const toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent(selectionColumnID, (int) rowIndex)))
        {
            toggle->setToggleState(rows[rowIndex].selected, juce::dontSendNotification);

            toggle->onClick = [this, rowIndex, toggle]
            {
                rows[rowIndex].selected = toggle->getToggleState();
                selectionChanged();
            };
        }

        if (auto* const nameAndSerialNumber = dynamic_cast<SimpleLabel*>(table.getCellComponent(nameAndSerialNumberColumnID, (int) rowIndex)))
        {
            nameAndSerialNumber->setText(rows[rowIndex].getNameAndSerialNumber());
        }

        if (auto* const info = dynamic_cast<SimpleLabel*>(table.getCellComponent(infoColumnID, (int) rowIndex)))
        {
            info->setText(rows[rowIndex].createConnectionInfo()->toString());
        }

        numberOfConnectionsPerType[rows[rowIndex].connectionType]++;
    }

    juce::String labelText("Found: ");
    if (numberOfConnectionsPerType.empty())
    {
        labelText += "0 Connections";
    }
    else
    {
        for (auto it = numberOfConnectionsPerType.begin(); it != numberOfConnectionsPerType.end(); it++)
        {
            if (it != numberOfConnectionsPerType.begin())
            {
                labelText += ", ";
            }
            labelText += juce::String(it->second) + " " + juce::String(XIMU3_connection_type_to_string(it->first));
        }
    }
    numConnectionsFoundLabel.setText(labelText);
}

std::vector<std::unique_ptr<ximu3::ConnectionInfo>> DiscoveredDevicesTable::getConnectionInfos() const
{
    std::vector<std::unique_ptr<ximu3::ConnectionInfo>> connectionInfos;
    for (auto& row : rows)
    {
        if (row.selected)
        {
            connectionInfos.push_back(row.createConnectionInfo());
        }
    }
    return connectionInfos;
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
    jassert (rowNumber >= 0 && rowNumber < getNumRows());

    if (rowNumber >= (int) rows.size())
    {
        delete existingComponentToUpdate;
        return nullptr;
    }

    if (existingComponentToUpdate == nullptr)
    {
        switch (columnID)
        {
            case selectionColumnID:
            {
                auto* toggle = new CustomToggleButton("");
                toggle->setWantsKeyboardFocus(false);
                return toggle;
            }

            case nameAndSerialNumberColumnID:
            case infoColumnID:
                return new SimpleLabel();

            default:
                jassertfalse;
        }
    }

    return existingComponentToUpdate;
}

void DiscoveredDevicesTable::cellClicked(int rowNumber, int, const juce::MouseEvent&)
{
    if (auto* toggle = dynamic_cast<CustomToggleButton*>(table.getCellComponent(selectionColumnID, rowNumber)))
    {
        toggle->setToggleState(!rows[(size_t) rowNumber].selected, juce::sendNotificationSync);
    }
}
