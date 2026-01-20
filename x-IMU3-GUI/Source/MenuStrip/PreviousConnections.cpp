#include "PreviousConnections.h"

PreviousConnections::PreviousConnections() {
    connections = juce::ValueTree::fromXml(file.loadFileAsString());
    if (connections.isValid() == false) {
        connections = juce::ValueTree("Connections");
    }
}

void PreviousConnections::update(const ximu3::ConnectionConfig &config) {
    if (const auto *usbConnectionConfig = dynamic_cast<const ximu3::UsbConnectionConfig *>(&config)) {
        update({"UsbConnectionConfig", {{"portName", usbConnectionConfig->port_name}}});
        return;
    }

    if (const auto *serialConnectionConfig = dynamic_cast<const ximu3::SerialConnectionConfig *>(&config)) {
        update({
            "SerialConnectionConfig", {
                {"portName", serialConnectionConfig->port_name},
                {"baudRate", (int) serialConnectionConfig->baud_rate},
                {"rtsCtsEnabled", serialConnectionConfig->rts_cts_enabled ? 1 : 0}
            }
        });
        return;
    }

    if (const auto *tcpConnectionConfig = dynamic_cast<const ximu3::TcpConnectionConfig *>(&config)) {
        update({
            "TcpConnectionConfig", {
                {"ipAddress", tcpConnectionConfig->ip_address},
                {"port", (int) tcpConnectionConfig->port}
            }
        });
        return;
    }

    if (const auto *udpConnectionConfig = dynamic_cast<const ximu3::UdpConnectionConfig *>(&config)) {
        update({
            "UdpConnectionConfig", {
                {"ipAddress", udpConnectionConfig->ip_address},
                {"sendPort", (int) udpConnectionConfig->send_port},
                {"receivePort", (int) udpConnectionConfig->receive_port}
            }
        });
        return;
    }

    if (const auto *bluetoothConnectionConfig = dynamic_cast<const ximu3::BluetoothConnectionConfig *>(&config)) {
        update({"BluetoothConnectionConfig", {{"portName", bluetoothConnectionConfig->port_name}}});
    }
}

void PreviousConnections::update(std::pair<std::uint8_t, std::uint8_t> muxChannels) {
    update({"MuxConnectionConfigs", {{"firstChannel", muxChannels.first}, {"lastChannel", muxChannels.second}}});
}

std::vector<std::variant<std::unique_ptr<ximu3::ConnectionConfig>, std::pair<std::uint8_t, std::uint8_t> > > PreviousConnections::get() const {
    std::vector<std::variant<std::unique_ptr<ximu3::ConnectionConfig>, std::pair<std::uint8_t, std::uint8_t> > > result;

    for (auto config: connections) {
        if (config.hasType("UsbConnectionConfig")) {
            result.push_back(std::make_unique<ximu3::UsbConnectionConfig>(config["portName"].toString().toStdString()));
            continue;
        }

        if (config.hasType("SerialConnectionConfig")) {
            result.push_back(std::make_unique<ximu3::SerialConnectionConfig>(config["portName"].toString().toStdString(),
                                                                             (uint32_t) ((int) config["baudRate"]),
                                                                             config["rtsCtsEnabled"]));
            continue;
        }

        if (config.hasType("TcpConnectionConfig")) {
            result.push_back(std::make_unique<ximu3::TcpConnectionConfig>(config["ipAddress"].toString().toStdString(),
                                                                          (uint16_t) ((int) config["port"])));
            continue;
        }

        if (config.hasType("UdpConnectionConfig")) {
            result.push_back(std::make_unique<ximu3::UdpConnectionConfig>(config["ipAddress"].toString().toStdString(),
                                                                          (uint16_t) ((int) config["sendPort"]),
                                                                          (uint16_t) ((int) config["receivePort"])));
            continue;
        }

        if (config.hasType("BluetoothConnectionConfig")) {
            result.push_back(std::make_unique<ximu3::BluetoothConnectionConfig>(config["portName"].toString().toStdString()));
            continue;
        }

        if (config.hasType("MuxConnectionConfigs")) {
            result.push_back(std::pair{static_cast<std::uint8_t>((int) config["firstChannel"]), static_cast<std::uint8_t>((int) config["lastChannel"])});
        }
    }

    return result;
}

void PreviousConnections::update(juce::ValueTree newChild) {
    for (auto child: connections) {
        if (child.isEquivalentTo(newChild)) {
            connections.removeChild(child, nullptr);
            break;
        }
    }

    while (connections.getNumChildren() >= 6) {
        connections.removeChild(connections.getChild(connections.getNumChildren() - 1), nullptr);
    }

    connections.addChild(newChild, 0, nullptr);
    file.create();
    file.replaceWithText(connections.toXmlString());
}
