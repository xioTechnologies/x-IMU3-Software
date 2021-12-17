#include "../ApplicationSettings.h"
#include "ApplicationSettingsDialog.h"
#include "NetworkDiscoveryDispatcher.h"
#include "SearchForConnectionsDialog.h"

SearchForConnectionsDialog::SearchForConnectionsDialog(std::vector<std::unique_ptr<ximu3::ConnectionInfo>> existingConnections_) : Dialog(BinaryData::search_svg, "Search For Connections", "Connect", "Cancel", &filterButton, 50, true),
                                                                                                                                   existingConnections(std::move(existingConnections_))
{
    addAndMakeVisible(table);
    addAndMakeVisible(filterButton);

    table.onSelectionChanged = [this]
    {
        for (auto& device : devices)
        {
            if (device.selected)
            {
                setValid(true);
                return;
            }
        }

        setValid(false);
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

const std::vector<DiscoveredDevicesTable::DiscoveredDevice>& SearchForConnectionsDialog::getDiscoveredDevices() const
{
    return devices;
}

void SearchForConnectionsDialog::update()
{
    std::vector<DiscoveredDevicesTable::DiscoveredDevice> newDevices;

    // Create filtered devices
    for (auto& serialDevice : serialDevices)
    {
        if (((serialDevice.connection_type == ximu3::XIMU3_ConnectionTypeUsb) && ApplicationSettings::getSingleton().searchUsb) ||
            ((serialDevice.connection_type == ximu3::XIMU3_ConnectionTypeSerial) && ApplicationSettings::getSingleton().searchSerial) ||
            ((serialDevice.connection_type == ximu3::XIMU3_ConnectionTypeBluetooth) && ApplicationSettings::getSingleton().searchBluetooth))
        {
            DiscoveredDevicesTable::DiscoveredDevice device;
            device.deviceNameAndSerialNumber = juce::String(serialDevice.device_name) + " - " + juce::String(serialDevice.serial_number);
            device.connectionType = serialDevice.connection_type;

            switch (serialDevice.connection_type)
            {
                case ximu3::XIMU3_ConnectionTypeUsb:
                    device.connectionInfo = std::make_unique<ximu3::UsbConnectionInfo>(serialDevice.usb_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeSerial:
                    device.connectionInfo = std::make_unique<ximu3::SerialConnectionInfo>(serialDevice.serial_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeBluetooth:
                    device.connectionInfo = std::make_unique<ximu3::BluetoothConnectionInfo>(serialDevice.bluetooth_connection_info);
                    break;
                case ximu3::XIMU3_ConnectionTypeTcp:
                case ximu3::XIMU3_ConnectionTypeUdp:
                case ximu3::XIMU3_ConnectionTypeFile:
                    break;
            }

            newDevices.push_back(std::move(device));
        }
    }

    for (auto& networkDevice : networkDiscoveryDispatcher.getDevices())
    {
        DiscoveredDevicesTable::DiscoveredDevice device;
        device.deviceNameAndSerialNumber = juce::String(networkDevice.device_name) + " - " + juce::String(networkDevice.serial_number);

        if (ApplicationSettings::getSingleton().searchUdp)
        {
            device.connectionType = ximu3::XIMU3_ConnectionTypeUdp;
            device.connectionInfo = std::make_unique<ximu3::UdpConnectionInfo>(networkDevice.udp_connection_info);
        }
        if (ApplicationSettings::getSingleton().searchTcp)
        {
            device.connectionType = ximu3::XIMU3_ConnectionTypeTcp;
            device.connectionInfo = std::make_unique<ximu3::TcpConnectionInfo>(networkDevice.tcp_connection_info);
        }

        newDevices.push_back(std::move(device));
    }

    // Remove existing connections
    for (size_t index = 0; index < newDevices.size(); index++)
    {
        for (const auto& connection : existingConnections)
        {
            if (connection->toString() == newDevices[index].connectionInfo->toString())
            {
                newDevices.erase(newDevices.begin() + (int) index--);
            }
        }
    }

    // Preserve selected state
    for (auto& newDevice : newDevices)
    {
        for (auto& device : devices)
        {
            if (newDevice.connectionInfo->toString() == device.connectionInfo->toString())
            {
                newDevice.selected = device.selected;
            }
        }
    }

    // Update table
    devices = std::move(newDevices);
    table.update();
}

juce::PopupMenu SearchForConnectionsDialog::getFilterMenu()
{
    juce::PopupMenu menu;
    const auto addFilterItem = [&](const auto& name, juce::CachedValue<bool>& value)
    {
        menu.addItem(name, true, value, [this, &value]
        {
            value = !value;
            ApplicationSettings::getSingleton().save();
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
