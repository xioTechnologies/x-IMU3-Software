#include "../ApplicationSettings.h"
#include "ApplicationSettingsDialog.h"
#include "SearchForConnectionsDialog.h"

SearchForConnectionsDialog::SearchForConnectionsDialog(std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections_)
        : Dialog(BinaryData::search_svg, "", "Connect", "Cancel", &filterButton, 50, true),
          existingConnections(std::move(existingConnections_))
{
    addAndMakeVisible(table);
    addAndMakeVisible(filterButton);

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
    std::vector<ConnectionsTable::Row> rows;
    std::map<ximu3::XIMU3_ConnectionType, int> numberOfConnections;

    const auto addConnection = [&](const auto& deviceName, const auto& serialNumber, const auto& connectionInfo, const auto& connectionType)
    {
        for (const auto& existingConnection : existingConnections)
        {
            if (existingConnection->toString() == connectionInfo->toString())
            {
                return;
            }
        }

        rows.push_back({ false, deviceName, serialNumber, std::move(connectionInfo), connectionType });
        numberOfConnections[connectionType]++;
    };

    for (const auto& device : portScanner.getDevices())
    {
        if (device.connection_type == ximu3::XIMU3_ConnectionTypeUsb && ApplicationSettings::getSingleton().searchUsb)
        {
            addConnection(device.device_name, device.serial_number, std::make_shared<ximu3::UsbConnectionInfo>(device.usb_connection_info), device.connection_type);
        }
        else if (device.connection_type == ximu3::XIMU3_ConnectionTypeSerial && ApplicationSettings::getSingleton().searchSerial)
        {
            addConnection(device.device_name, device.serial_number, std::make_shared<ximu3::SerialConnectionInfo>(device.serial_connection_info), device.connection_type);
        }
        else if (device.connection_type == ximu3::XIMU3_ConnectionTypeBluetooth && ApplicationSettings::getSingleton().searchBluetooth)
        {
            addConnection(device.device_name, device.serial_number, std::make_shared<ximu3::BluetoothConnectionInfo>(device.bluetooth_connection_info), device.connection_type);
        }
    }

    for (const auto& message : networkAnnouncement->getMessages())
    {
        if (ApplicationSettings::getSingleton().searchUdp)
        {
            addConnection(message.device_name, message.serial_number, std::make_shared<ximu3::UdpConnectionInfo>(message.udp_connection_info), ximu3::XIMU3_ConnectionTypeUdp);
        }
        if (ApplicationSettings::getSingleton().searchTcp)
        {
            addConnection(message.device_name, message.serial_number, std::make_shared<ximu3::TcpConnectionInfo>(message.tcp_connection_info), ximu3::XIMU3_ConnectionTypeTcp);
        }
    }

    table.setRows(rows);

    juce::String numberOfConnectionsText;
    for (const auto& pair : numberOfConnections)
    {
        numberOfConnectionsText += juce::String(pair.second) + " " + juce::String(XIMU3_connection_type_to_string(pair.first)) + ", ";
    }
    getTopLevelComponent()->setName("Search for Connections" + (numberOfConnectionsText.isEmpty() ? "" : (" (" + numberOfConnectionsText.dropLastCharacters(2) + ")")));

    setValid(getConnectionInfos().empty() == false);
}
