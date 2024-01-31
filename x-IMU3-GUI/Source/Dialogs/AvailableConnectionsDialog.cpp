#include "ApplicationSettings.h"
#include "ApplicationSettingsDialog.h"
#include "AvailableConnectionsDialog.h"

AvailableConnectionsDialog::AvailableConnectionsDialog(std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections_)
        : Dialog(BinaryData::search_svg, "", "Connect", "Cancel", &filterButton, iconButtonWidth, true),
          existingConnections(std::move(existingConnections_))
{
    addAndMakeVisible(table);
    addAndMakeVisible(filterButton);

    filterButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(6));

    startTimerHz(10);
}

void AvailableConnectionsDialog::resized()
{
    Dialog::resized();
    table.setBounds(getContentBounds(true));
}

std::vector<ximu3::ConnectionInfo*> AvailableConnectionsDialog::getConnectionInfos() const
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

juce::PopupMenu AvailableConnectionsDialog::getFilterMenu()
{
    juce::PopupMenu menu;
    const auto addFilterItem = [&](const auto& name, juce::CachedValue<bool>& value)
    {
        menu.addItem(name, true, value, [&value]
        {
            value = !value;
        });
    };
    addFilterItem("USB", ApplicationSettings::getSingleton().availableConnections.usb);
    addFilterItem("Serial", ApplicationSettings::getSingleton().availableConnections.serial);
    addFilterItem("TCP", ApplicationSettings::getSingleton().availableConnections.tcp);
    addFilterItem("UDP", ApplicationSettings::getSingleton().availableConnections.udp);
    addFilterItem("Bluetooth", ApplicationSettings::getSingleton().availableConnections.bluetooth);
    return menu;
}

void AvailableConnectionsDialog::timerCallback()
{
    std::vector<ConnectionsTable::Row> rows;
    std::map<ximu3::XIMU3_ConnectionType, int> numberOfConnections;

    const auto addConnection = [&](const auto& deviceName, const auto& serialNumber, const auto& connectionInfo, const auto& connectionType, const std::optional<int>& rssiPercentage, const std::optional<int>& batteryPercentage, const std::optional<ximu3::XIMU3_ChargingStatus>& batteryStatus)
    {
        for (const auto& existingConnection : existingConnections)
        {
            if (existingConnection->toString() == connectionInfo->toString())
            {
                return;
            }
        }

        rows.push_back({ false, deviceName, serialNumber, std::move(connectionInfo), connectionType, rssiPercentage, batteryPercentage, batteryStatus });
        numberOfConnections[connectionType]++;
    };

    for (const auto& device : portScanner.getDevices())
    {
        if (device.connection_type == ximu3::XIMU3_ConnectionTypeUsb && ApplicationSettings::getSingleton().availableConnections.usb)
        {
            addConnection(device.device_name, device.serial_number, std::make_shared<ximu3::UsbConnectionInfo>(device.usb_connection_info), device.connection_type, {}, {}, {});
        }
        else if (device.connection_type == ximu3::XIMU3_ConnectionTypeSerial && ApplicationSettings::getSingleton().availableConnections.serial)
        {
            addConnection(device.device_name, device.serial_number, std::make_shared<ximu3::SerialConnectionInfo>(device.serial_connection_info), device.connection_type, {}, {}, {});
        }
        else if (device.connection_type == ximu3::XIMU3_ConnectionTypeBluetooth && ApplicationSettings::getSingleton().availableConnections.bluetooth)
        {
            addConnection(device.device_name, device.serial_number, std::make_shared<ximu3::BluetoothConnectionInfo>(device.bluetooth_connection_info), device.connection_type, {}, {}, {});
        }
    }

    for (const auto& message : networkAnnouncement->getMessages())
    {
        if (ApplicationSettings::getSingleton().availableConnections.udp)
        {
            addConnection(message.device_name, message.serial_number, std::make_shared<ximu3::UdpConnectionInfo>(ximu3::XIMU3_network_announcement_message_to_udp_connection_info(message)), ximu3::XIMU3_ConnectionTypeUdp, message.rssi, message.battery, message.charging_status);
        }
        if (ApplicationSettings::getSingleton().availableConnections.tcp)
        {
            addConnection(message.device_name, message.serial_number, std::make_shared<ximu3::TcpConnectionInfo>(ximu3::XIMU3_network_announcement_message_to_tcp_connection_info(message)), ximu3::XIMU3_ConnectionTypeTcp, message.rssi, message.battery, message.charging_status);
        }
    }

    table.setRows(rows);

    juce::String numberOfConnectionsText;
    for (const auto& pair : numberOfConnections)
    {
        numberOfConnectionsText += juce::String(pair.second) + " " + juce::String(XIMU3_connection_type_to_string(pair.first)) + ", ";
    }
    getTopLevelComponent()->setName("Available Connections" + (numberOfConnectionsText.isEmpty() ? "" : (" (" + numberOfConnectionsText.dropLastCharacters(2) + ")")));

    setOkButton(getConnectionInfos().empty() == false);
}
