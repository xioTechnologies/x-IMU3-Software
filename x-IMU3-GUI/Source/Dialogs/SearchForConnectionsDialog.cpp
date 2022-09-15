#include "../ApplicationSettings.h"
#include "ApplicationSettingsDialog.h"
#include "SearchForConnectionsDialog.h"

SearchForConnectionsDialog::SearchForConnectionsDialog(std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections_)
        : Dialog(BinaryData::search_svg, "", "Connect", "Cancel", &filterButton, 50, true),
          existingConnections(std::move(existingConnections_))
{
    addAndMakeVisible(table);
    addAndMakeVisible(filterButton);

    table.onSelectionChanged = [this]
    {
        setValid(getConnectionInfos().empty() == false);
    };

    filterButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(6));

    startTimerHz(10);
}

void SearchForConnectionsDialog::resized()
{
    Dialog::resized();
    table.setBounds(getContentBounds(true));
}

std::vector<ximu3::ConnectionInfo*> SearchForConnectionsDialog::getConnectionInfos() const
{
    std::vector<ximu3::ConnectionInfo*> connectionInfos;

    for (auto& row : table.getRows())
    {
        if (row.selected)
        {
            connectionInfos.push_back(row.connectionInfo.get());
        }
    }

    return connectionInfos;
}

juce::PopupMenu SearchForConnectionsDialog::getFilterMenu()
{
    juce::PopupMenu menu;
    const auto addFilterItem = [&](const auto& name, juce::CachedValue<bool>& value)
    {
        menu.addItem(name, true, value, [&value]
        {
            value = !value;
        });
    };
    addFilterItem("USB", ApplicationSettings::getSingleton().searchUsb);
    addFilterItem("Serial", ApplicationSettings::getSingleton().searchSerial);
    addFilterItem("TCP", ApplicationSettings::getSingleton().searchTcp);
    addFilterItem("UDP", ApplicationSettings::getSingleton().searchUdp);
    addFilterItem("Bluetooth", ApplicationSettings::getSingleton().searchBluetooth);
    return menu;
}

void SearchForConnectionsDialog::timerCallback()
{
    std::vector<DiscoveredDevicesTable::Row> rows;

    const auto addRow = [&](auto row)
    {
        for (const auto& existingConnection : existingConnections)
        {
            if (existingConnection->toString() == row.connectionInfo->toString())
            {
                return;
            }
        }
        rows.push_back(std::move(row));
    };

    for (const auto& device : portScanner.getDevices())
    {
        if (((device.connection_type == ximu3::XIMU3_ConnectionTypeUsb) && ApplicationSettings::getSingleton().searchUsb) ||
            ((device.connection_type == ximu3::XIMU3_ConnectionTypeSerial) && ApplicationSettings::getSingleton().searchSerial) ||
            ((device.connection_type == ximu3::XIMU3_ConnectionTypeBluetooth) && ApplicationSettings::getSingleton().searchBluetooth))
        {
            DiscoveredDevicesTable::Row row;
            row.deviceNameAndSerialNumber = juce::String(device.device_name) + " - " + juce::String(device.serial_number);

            switch (device.connection_type)
            {
                case ximu3::XIMU3_ConnectionTypeUsb:
                    row.connectionInfo = std::make_unique<ximu3::UsbConnectionInfo>(device.usb_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeSerial:
                    row.connectionInfo = std::make_unique<ximu3::SerialConnectionInfo>(device.serial_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeBluetooth:
                    row.connectionInfo = std::make_unique<ximu3::BluetoothConnectionInfo>(device.bluetooth_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeTcp:
                case ximu3::XIMU3_ConnectionTypeUdp:
                case ximu3::XIMU3_ConnectionTypeFile:
                    break;
            }
            row.connectionType = device.connection_type;
            addRow(std::move(row));
        }
    }

    for (const auto& message : networkAnnouncement->getMessages())
    {
        if (ApplicationSettings::getSingleton().searchUdp)
        {
            DiscoveredDevicesTable::Row row;
            row.deviceNameAndSerialNumber = juce::String(message.device_name) + " - " + juce::String(message.serial_number);
            row.connectionInfo = std::make_unique<ximu3::UdpConnectionInfo>(message.udp_connection_info);
            row.connectionType = ximu3::XIMU3_ConnectionTypeUdp;
            addRow(std::move(row));
        }
        if (ApplicationSettings::getSingleton().searchTcp)
        {
            DiscoveredDevicesTable::Row row;
            row.deviceNameAndSerialNumber = juce::String(message.device_name) + " - " + juce::String(message.serial_number);
            row.connectionInfo = std::make_unique<ximu3::TcpConnectionInfo>(message.tcp_connection_info);
            row.connectionType = ximu3::XIMU3_ConnectionTypeTcp;
            addRow(std::move(row));
        }
    }

    table.setRows(std::move(rows));

    std::map<ximu3::XIMU3_ConnectionType, int> numberOfConnections;
    for (const auto& row : table.getRows())
    {
        numberOfConnections[row.connectionType]++;
    }

    juce::String numberOfConnectionsText;
    for (const auto& pair : numberOfConnections)
    {
        numberOfConnectionsText += juce::String(pair.second) + " " + juce::String(XIMU3_connection_type_to_string(pair.first)) + ", ";
    }
    numberOfConnectionsText = numberOfConnectionsText.dropLastCharacters(2);
    if (numberOfConnectionsText.isNotEmpty())
    {
        numberOfConnectionsText = " (" + numberOfConnectionsText + ")";
    }

    getTopLevelComponent()->setName("Search for Connections" + numberOfConnectionsText);
}
