#include "ApplicationSettings.h"
#include "AvailableConnectionsDialog.h"

AvailableConnectionsDialog::AvailableConnectionsDialog(std::vector<ExistingConnection> existingConnections_)
    : Dialog(BinaryData::search_svg, "", "Connect", "Cancel", &filterButton, iconButtonWidth, true),
      existingConnections(existingConnections_) {
    addAndMakeVisible(table);
    addAndMakeVisible(filterButton);

    filterButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(6));

    startTimerHz(10);
    timerCallback();
}

void AvailableConnectionsDialog::resized() {
    Dialog::resized();
    table.setBounds(getContentBounds(true));
}

std::vector<ximu3::ConnectionConfig *> AvailableConnectionsDialog::getConnectionConfigs() const {
    std::vector<ximu3::ConnectionConfig *> configs;

    for (auto &row: table.getRows()) {
        if (row.selected && row.unavailable == false) {
            configs.push_back(row.config.get());
        }
    }

    return configs;
}

juce::PopupMenu AvailableConnectionsDialog::getFilterMenu() {
    juce::PopupMenu menu;
    const auto addFilterItem = [&](const auto &name, juce::CachedValue<bool> &value) {
        menu.addItem(name, true, value, [&value] {
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

void AvailableConnectionsDialog::timerCallback() {
    std::vector<ConnectionsTable::Row> rows;
    std::map<juce::String, int> numberOfConnections;

    static const auto toString = [](const ximu3::ConnectionConfig &config) {
        if (dynamic_cast<const ximu3::XIMU3_UsbConnectionConfig *>(&config) != nullptr) {
            return "USB";
        }
        if (dynamic_cast<const ximu3::XIMU3_SerialConnectionConfig *>(&config) != nullptr) {
            return "Serial";
        }
        if (dynamic_cast<const ximu3::XIMU3_TcpConnectionConfig *>(&config) != nullptr) {
            return "TCP";
        }
        if (dynamic_cast<const ximu3::XIMU3_UdpConnectionConfig *>(&config) != nullptr) {
            return "UDP";
        }
        if (dynamic_cast<const ximu3::XIMU3_BluetoothConnectionConfig *>(&config) != nullptr) {
            return "Bluetooth";
        }
        jassertfalse;
        return "";
    };

    static const auto filter = [](const ximu3::ConnectionConfig &config) {
        if (dynamic_cast<const ximu3::XIMU3_UsbConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().availableConnections.usb;
        }
        if (dynamic_cast<const ximu3::XIMU3_SerialConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().availableConnections.serial;
        }
        if (dynamic_cast<const ximu3::XIMU3_TcpConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().availableConnections.tcp;
        }
        if (dynamic_cast<const ximu3::XIMU3_UdpConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().availableConnections.udp;
        }
        if (dynamic_cast<const ximu3::XIMU3_BluetoothConnectionConfig *>(&config) != nullptr) {
            return *ApplicationSettings::getSingleton().availableConnections.bluetooth;
        }
        jassertfalse;
        return false;
    };

    const auto addRow = [&](const auto &descriptor, const auto &config, const std::optional<int> &rssi, const std::optional<int> &battery, const std::optional<ximu3::XIMU3_ChargingStatus> &status) {
        if (filter(*config) == false || std::find(existingConnections.begin(), existingConnections.end(), *config) != existingConnections.end()) {
            return;
        }

        rows.push_back({false, descriptor, config, rssi, battery, status, false});
        numberOfConnections[toString(*config)]++;
    };

    for (const auto &device: portScanner.getDevices()) {
        addRow(juce::String(device.device_name) + " " + device.serial_number, ximu3::ConnectionConfig::from(device), {}, {}, {});
    }

    for (const auto &message: networkAnnouncement->getMessages()) {
        if (message.tcp_port != 0) {
            addRow(juce::String(message.device_name) + " " + message.serial_number, std::make_shared<ximu3::TcpConnectionConfig>(message), message.rssi, message.battery, message.charging_status);
        }
        if ((message.udp_send != 0) && (message.udp_receive != 0)) {
            addRow(juce::String(message.device_name) + " " + message.serial_number, std::make_shared<ximu3::UdpConnectionConfig>(message), message.rssi, message.battery, message.charging_status);
        }
    }

    for (const auto &existingConnection: existingConnections) {
        rows.push_back({true, existingConnection.descriptor, existingConnection.config, {}, {}, {}, true});
    }

    table.setRows(rows);

    juce::String numberOfConnectionsText;
    for (const auto &pair: numberOfConnections) {
        numberOfConnectionsText += juce::String(pair.second) + " " + juce::String(pair.first) + ", ";
    }
    getTopLevelComponent()->setName("Available Connections" + (numberOfConnectionsText.isEmpty() ? "" : (" (" + numberOfConnectionsText.dropLastCharacters(2) + ")")));

    setOkButton(getConnectionConfigs().empty() == false);
}
