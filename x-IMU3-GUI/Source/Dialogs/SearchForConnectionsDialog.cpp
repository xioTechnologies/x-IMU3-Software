#include "../ApplicationSettings.h"
#include "ApplicationSettingsDialog.h"
#include "NetworkDiscoveryDispatcher.h"
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

    update();

    filterButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(6));

    networkDiscoveryDispatcher.addChangeListener(this);
}

SearchForConnectionsDialog::~SearchForConnectionsDialog()
{
    networkDiscoveryDispatcher.removeChangeListener(this);
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

void SearchForConnectionsDialog::update()
{
    std::vector<DiscoveredDevicesTable::Row> rows;

    const auto add = [&](auto row)
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

    for (const auto& serialDevice : serialDevices)
    {
        if (((serialDevice.connection_type == ximu3::XIMU3_ConnectionTypeUsb) && ApplicationSettings::getSingleton().searchUsb) ||
            ((serialDevice.connection_type == ximu3::XIMU3_ConnectionTypeSerial) && ApplicationSettings::getSingleton().searchSerial) ||
            ((serialDevice.connection_type == ximu3::XIMU3_ConnectionTypeBluetooth) && ApplicationSettings::getSingleton().searchBluetooth))
        {
            DiscoveredDevicesTable::Row row;
            row.deviceNameAndSerialNumber = juce::String(serialDevice.device_name) + " - " + juce::String(serialDevice.serial_number);

            switch (serialDevice.connection_type)
            {
                case ximu3::XIMU3_ConnectionTypeUsb:
                    row.connectionInfo = std::make_unique<ximu3::UsbConnectionInfo>(serialDevice.usb_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeSerial:
                    row.connectionInfo = std::make_unique<ximu3::SerialConnectionInfo>(serialDevice.serial_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeBluetooth:
                    row.connectionInfo = std::make_unique<ximu3::BluetoothConnectionInfo>(serialDevice.bluetooth_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeTcp:
                case ximu3::XIMU3_ConnectionTypeUdp:
                case ximu3::XIMU3_ConnectionTypeFile:
                    break;
            }
            row.connectionType = serialDevice.connection_type;
            add(std::move(row));
        }
    }

    for (const auto& networkDevice : networkDiscoveryDispatcher.getDevices())
    {
        if (ApplicationSettings::getSingleton().searchUdp)
        {
            DiscoveredDevicesTable::Row row;
            row.deviceNameAndSerialNumber = juce::String(networkDevice.device_name) + " - " + juce::String(networkDevice.serial_number);
            row.connectionInfo = std::make_unique<ximu3::UdpConnectionInfo>(networkDevice.udp_connection_info);
            row.connectionType = ximu3::XIMU3_ConnectionTypeUdp;
            add(std::move(row));
        }
        if (ApplicationSettings::getSingleton().searchTcp)
        {
            DiscoveredDevicesTable::Row row;
            row.deviceNameAndSerialNumber = juce::String(networkDevice.device_name) + " - " + juce::String(networkDevice.serial_number);
            row.connectionInfo = std::make_unique<ximu3::TcpConnectionInfo>(networkDevice.tcp_connection_info);
            row.connectionType = ximu3::XIMU3_ConnectionTypeTcp;
            add(std::move(row));
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

juce::PopupMenu SearchForConnectionsDialog::getFilterMenu()
{
    juce::PopupMenu menu;
    const auto addFilterItem = [&](const auto& name, juce::CachedValue<bool>& value)
    {
        menu.addItem(name, true, value, [this, &value]
        {
            value = !value;
            update();
        });
    };
    addFilterItem("USB", ApplicationSettings::getSingleton().searchUsb);
    addFilterItem("Serial", ApplicationSettings::getSingleton().searchSerial);
    addFilterItem("TCP", ApplicationSettings::getSingleton().searchTcp);
    addFilterItem("UDP", ApplicationSettings::getSingleton().searchUdp);
    addFilterItem("Bluetooth", ApplicationSettings::getSingleton().searchBluetooth);
    return menu;
}

void SearchForConnectionsDialog::changeListenerCallback(juce::ChangeBroadcaster*)
{
    update();
}
