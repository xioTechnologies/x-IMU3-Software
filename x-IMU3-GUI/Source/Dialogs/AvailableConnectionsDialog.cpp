#include "ApplicationSettings.h"
#include "AvailableConnectionsDialog.h"

AvailableConnectionsDialog::AvailableConnectionsDialog(std::vector<ExistingConnection> existingConnections_)
    : Dialog(BinaryData::search_svg, "", "Connect", "Cancel", &filterButton, iconButtonWidth, true),
      existingConnections(existingConnections_)
{
    addAndMakeVisible(table);
    addAndMakeVisible(filterButton);

    filterButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(6));

    startTimerHz(10);
    timerCallback();
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
        if (row.selected && row.unavailable == false)
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

    static const auto filter = [](ximu3::XIMU3_ConnectionType connectionType)
    {
        switch (connectionType)
        {
            case ximu3::XIMU3_ConnectionTypeUsb:
                return ApplicationSettings::getSingleton().availableConnections.usb == false;
            case ximu3::XIMU3_ConnectionTypeSerial:
                return ApplicationSettings::getSingleton().availableConnections.serial == false;
            case ximu3::XIMU3_ConnectionTypeTcp:
                return ApplicationSettings::getSingleton().availableConnections.tcp == false;
            case ximu3::XIMU3_ConnectionTypeUdp:
                return ApplicationSettings::getSingleton().availableConnections.udp == false;
            case ximu3::XIMU3_ConnectionTypeBluetooth:
                return ApplicationSettings::getSingleton().availableConnections.bluetooth == false;
            case ximu3::XIMU3_ConnectionTypeFile:
                break;
        }
        return false;
    };

    const auto addRow = [&](const auto& descriptor, const auto& connectionInfo, const std::optional<int>& rssi, const std::optional<int>& battery, const std::optional<ximu3::XIMU3_ChargingStatus>& status)
    {
        if (filter(connectionInfo->getType()) || std::find(existingConnections.begin(), existingConnections.end(), *connectionInfo) != existingConnections.end())
        {
            return;
        }

        rows.push_back({ false, descriptor, connectionInfo, rssi, battery, status, false });
        numberOfConnections[connectionInfo->getType()]++;
    };

    for (const auto& device : portScanner.getDevices())
    {
        addRow(juce::String(device.device_name) + " " + device.serial_number, ximu3::ConnectionInfo::from(device), {}, {}, {});
    }

    for (const auto& message : networkAnnouncement->getMessages())
    {
        addRow(juce::String(message.device_name) + " " + message.serial_number, std::make_shared<ximu3::TcpConnectionInfo>(message), message.rssi, message.battery, message.charging_status);
        addRow(juce::String(message.device_name) + " " + message.serial_number, std::make_shared<ximu3::UdpConnectionInfo>(message), message.rssi, message.battery, message.charging_status);
    }

    for (const auto& existingConnection : existingConnections)
    {
        rows.push_back({ true, existingConnection.descriptor, existingConnection.connectionInfo, {}, {}, {}, true });
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
