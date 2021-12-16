#include "../ApplicationSettings.h"
#include "ApplicationSettingsDialog.h"
#include "NetworkDiscoveryDispatcher.h"
#include "SearchForConnectionsDialog.h"

SearchForConnectionsDialog::SearchForConnectionsDialog() : Dialog(BinaryData::search_svg, "Search For Connections", "Connect", "Cancel", &filterButton, 50, true)
{
    addAndMakeVisible(table);
    addAndMakeVisible(filterButton);

    table.onSelectionChanged = [this]
    {
        setValid(table.getConnectionInfos().size() > 0);
    };

    updateTable();

    filterButton.setWantsKeyboardFocus(false);

    setValid(false);

    setSize(dialogWidth, calculateHeight(6));

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

std::vector<std::unique_ptr<ximu3::ConnectionInfo>> SearchForConnectionsDialog::getConnectionInfos() const
{
    return table.getConnectionInfos();
}

void SearchForConnectionsDialog::updateTable()
{
    std::vector<ximu3::XIMU3_DiscoveredSerialDevice> discoveredSerialDevicesCopy;

    {
        std::lock_guard<std::mutex> lock(devicesMutex);
        discoveredSerialDevicesCopy = devices;
    }

    DiscoveredDevicesTable::Filter filter;
    filter.usb = ApplicationSettings::getSingleton().searchUsb;
    filter.serial = ApplicationSettings::getSingleton().searchSerial;
    filter.tcp = ApplicationSettings::getSingleton().searchTcp;
    filter.udp = ApplicationSettings::getSingleton().searchUdp;
    filter.bluetooth = ApplicationSettings::getSingleton().searchBluetooth;

    table.updateDiscoveredDevices(discoveredSerialDevicesCopy, networkDiscoveryDispatcher.getDevices(), filter);
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
            updateTable();
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
    updateTable();
}

void SearchForConnectionsDialog::handleAsyncUpdate()
{
    updateTable();
}
