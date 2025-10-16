#include "PreviousConnections.h"

PreviousConnections::PreviousConnections()
{
    connections = juce::ValueTree::fromXml(file.loadFileAsString());
    if (connections.isValid() == false)
    {
        connections = juce::ValueTree("Connections");
    }
}

void PreviousConnections::update(const ximu3::ConnectionInfo& connectionInfo)
{
    if (const auto* usbConnectionInfo = dynamic_cast<const ximu3::UsbConnectionInfo*>(&connectionInfo))
    {
        update({ "UsbConnectionInfo", { { "portName", usbConnectionInfo->port_name } } });
        return;
    }

    if (const auto* serialConnectionInfo = dynamic_cast<const ximu3::SerialConnectionInfo*>(&connectionInfo))
    {
        update({
            "SerialConnectionInfo", {
                { "portName", serialConnectionInfo->port_name },
                { "baudRate", (int) serialConnectionInfo->baud_rate },
                { "rtsCtsEnabled", serialConnectionInfo->rts_cts_enabled ? 1 : 0 }
            }
        });
        return;
    }

    if (const auto* tcpConnectionInfo = dynamic_cast<const ximu3::TcpConnectionInfo*>(&connectionInfo))
    {
        update({
            "TcpConnectionInfo", {
                { "ipAddress", tcpConnectionInfo->ip_address },
                { "port", (int) tcpConnectionInfo->port }
            }
        });
        return;
    }

    if (const auto* udpConnectionInfo = dynamic_cast<const ximu3::UdpConnectionInfo*>(&connectionInfo))
    {
        update({
            "UdpConnectionInfo", {
                { "ipAddress", udpConnectionInfo->ip_address },
                { "sendPort", (int) udpConnectionInfo->send_port },
                { "receivePort", (int) udpConnectionInfo->receive_port }
            }
        });
        return;
    }

    if (const auto* bluetoothConnectionInfo = dynamic_cast<const ximu3::BluetoothConnectionInfo*>(&connectionInfo))
    {
        update({ "BluetoothConnectionInfo", { { "portName", bluetoothConnectionInfo->port_name } } });
    }
}

void PreviousConnections::update(std::pair<std::uint8_t, std::uint8_t> muxChannels)
{
    update({ "MuxConnectionInfos", { { "firstChannel", muxChannels.first }, { "lastChannel", muxChannels.second } } });
}

std::vector<std::variant<std::unique_ptr<ximu3::ConnectionInfo>, std::pair<std::uint8_t, std::uint8_t>>> PreviousConnections::get() const
{
    std::vector<std::variant<std::unique_ptr<ximu3::ConnectionInfo>, std::pair<std::uint8_t, std::uint8_t>>> result;

    for (auto connectionInfo : connections)
    {
        if (connectionInfo.hasType("UsbConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::UsbConnectionInfo>(connectionInfo["portName"].toString().toStdString()));
            continue;
        }

        if (connectionInfo.hasType("SerialConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::SerialConnectionInfo>(connectionInfo["portName"].toString().toStdString(),
                                                                           (uint32_t) ((int) connectionInfo["baudRate"]),
                                                                           connectionInfo["rtsCtsEnabled"]));
            continue;
        }

        if (connectionInfo.hasType("TcpConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::TcpConnectionInfo>(connectionInfo["ipAddress"].toString().toStdString(),
                                                                        (uint16_t) ((int) connectionInfo["port"])));
            continue;
        }

        if (connectionInfo.hasType("UdpConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::UdpConnectionInfo>(connectionInfo["ipAddress"].toString().toStdString(),
                                                                        (uint16_t) ((int) connectionInfo["sendPort"]),
                                                                        (uint16_t) ((int) connectionInfo["receivePort"])));
            continue;
        }

        if (connectionInfo.hasType("BluetoothConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::BluetoothConnectionInfo>(connectionInfo["portName"].toString().toStdString()));
            continue;
        }

        if (connectionInfo.hasType("MuxConnectionInfos"))
        {
            result.push_back(std::pair{ static_cast<std::uint8_t>((int) connectionInfo["firstChannel"]), static_cast<std::uint8_t>((int) connectionInfo["lastChannel"])  });
        }
    }

    return result;
}

void PreviousConnections::update(juce::ValueTree newChild)
{
    for (auto child : connections)
    {
        if (child.isEquivalentTo(newChild))
        {
            connections.removeChild(child, nullptr);
            break;
        }
    }

    while (connections.getNumChildren() >= 6)
    {
        connections.removeChild(connections.getChild(connections.getNumChildren() - 1), nullptr);
    }

    connections.addChild(newChild, 0, nullptr);
    file.create();
    file.replaceWithText(connections.toXmlString());
}
