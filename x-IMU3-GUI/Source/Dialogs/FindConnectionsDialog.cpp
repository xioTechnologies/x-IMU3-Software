#include "ApplicationSettings.h"
#include "FindConnectionsDialog.h"

FindConnectionsDialog::FindConnectionsDialog(std::vector<std::shared_ptr<ximu3::Connection> > existingConnections_)
    : Dialog(BinaryData::find_svg, "", "Connect", "Cancel", &filterButton, iconButtonWidth, true),
      existingConnections(existingConnections_) {
    addAndMakeVisible(table);
    addAndMakeVisible(filterButton);

    filterButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(6));

    startTimerHz(10);
    timerCallback();
}

void FindConnectionsDialog::resized() {
    Dialog::resized();
    table.setBounds(getContentBounds(true));
}

std::vector<ximu3::ConnectionConfig *> FindConnectionsDialog::getConnectionConfigs() const {
    std::vector<ximu3::ConnectionConfig *> configs;

    for (auto &row: table.getRows()) {
        if (row.selected && row.unavailable == false) {
            configs.push_back(row.config.get());
        }
    }

    return configs;
}

juce::PopupMenu FindConnectionsDialog::getFilterMenu() {
    juce::PopupMenu menu;
    const auto addFilterItem = [&](const auto &name, juce::CachedValue<bool> &value) {
        menu.addItem(name, true, value, [&value] {
            value = !value;
        });
    };
    addFilterItem("USB", ApplicationSettings::getSingleton().findConnections.usb);
    addFilterItem("Serial", ApplicationSettings::getSingleton().findConnections.serial);
    addFilterItem("TCP", ApplicationSettings::getSingleton().findConnections.tcp);
    addFilterItem("UDP", ApplicationSettings::getSingleton().findConnections.udp);
    addFilterItem("Bluetooth", ApplicationSettings::getSingleton().findConnections.bluetooth);
    addFilterItem("Mux", ApplicationSettings::getSingleton().findConnections.mux);
    return menu;
}

void FindConnectionsDialog::timerCallback() {
    std::vector<ConnectionsTable::Row> rows;
    std::map<juce::String, int> numberOfConnections;

    static const auto toString = [](const ximu3::ConnectionConfig &config) {
        if (dynamic_cast<const ximu3::UsbConnectionConfig *>(&config) != nullptr) {
            return "USB";
        }
        if (dynamic_cast<const ximu3::SerialConnectionConfig *>(&config) != nullptr) {
            return "Serial";
        }
        if (dynamic_cast<const ximu3::TcpConnectionConfig *>(&config) != nullptr) {
            return "TCP";
        }
        if (dynamic_cast<const ximu3::UdpConnectionConfig *>(&config) != nullptr) {
            return "UDP";
        }
        if (dynamic_cast<const ximu3::BluetoothConnectionConfig *>(&config) != nullptr) {
            return "Bluetooth";
        }
        if (dynamic_cast<const ximu3::MuxConnectionConfig *>(&config) != nullptr) {
            return "Mux";
        }
        jassertfalse;
        return "";
    };

    for (const auto &connection: existingConnections) {
        numberOfConnections[toString(*connection->getConfig())]++;
    }

    static const auto filter = [](const ximu3::ConnectionConfig &config) {
        if (dynamic_cast<const ximu3::UsbConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().findConnections.usb;
        }
        if (dynamic_cast<const ximu3::SerialConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().findConnections.serial;
        }
        if (dynamic_cast<const ximu3::TcpConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().findConnections.tcp;
        }
        if (dynamic_cast<const ximu3::UdpConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().findConnections.udp;
        }
        if (dynamic_cast<const ximu3::BluetoothConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().findConnections.bluetooth;
        }
        if (dynamic_cast<const ximu3::MuxConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().findConnections.mux;
        }
        jassertfalse;
        return false;
    };

    const auto addRow = [&](const auto &descriptor, auto &&config, const std::optional<int> &rssi, const std::optional<int> &battery, const std::optional<ximu3::XIMU3_ChargingStatus> &status) {
        if (filter(*config) == false || std::ranges::any_of(existingConnections, [&](const auto &connection) {
            return connection->getConfig()->toString() == config->toString();
        })) {
            return;
        }

        numberOfConnections[toString(*config)]++;
        rows.push_back({false, descriptor, std::move(config), rssi, battery, status, false});
    };

    for (const auto &device: portScanner.getDevices()) {
        addRow(juce::String(device.device_name) + " " + device.serial_number, ximu3::ConnectionConfig::from(device), {}, {}, {});
    }

    if (ApplicationSettings::getSingleton().findConnections.mux) {
        if (muxScanners.empty()) {
            for (const auto &connection: existingConnections) {
                muxScanners.push_back(std::make_unique<ximu3::MuxScanner>(*connection));
            }
        }

        for (auto &scanner: muxScanners) {
            for (const auto &device: scanner->getDevices()) {
                addRow(juce::String(device.device_name) + " " + device.serial_number, ximu3::ConnectionConfig::from(device), {}, {}, {});
            }
        }
    } else {
        muxScanners.clear();
    }

    for (const auto &message: networkAnnouncement->getMessages()) {
        for (const auto &device: ximu3::NetworkAnnouncement::toDevices(message)) {
            addRow(juce::String(message.device_name) + " " + message.serial_number, ximu3::ConnectionConfig::from(device), message.rssi, message.battery, message.charging_status);
        }
    }

    for (const auto &existingConnection: existingConnections) {
        juce::String descriptor;
        if (auto response = existingConnection->getPingResponse()) {
            descriptor = juce::String(response->device_name) + " " + juce::String(response->serial_number);
        }

        rows.push_back({true, descriptor, existingConnection->getConfig(), {}, {}, {}, true});
    }

    table.setRows(rows);

    juce::String numberOfConnectionsText;
    for (const auto &pair: numberOfConnections) {
        numberOfConnectionsText += juce::String(pair.second) + " " + juce::String(pair.first) + ", ";
    }
    getTopLevelComponent()->setName("Find Connections" + (numberOfConnectionsText.isEmpty() ? "" : (" (" + numberOfConnectionsText.dropLastCharacters(2) + ")")));

    setOkButton(getConnectionConfigs().empty() == false);
}
