#include "ConnectionHistory.h"

ConnectionHistory::ConnectionHistory()
{
    connectionHistory = juce::ValueTree::fromXml(file.loadFileAsString());
    if (connectionHistory.isValid() == false)
    {
        connectionHistory = juce::ValueTree("ConnectionHistory");
    }
}

void ConnectionHistory::update(const ximu3::ConnectionInfo& connectionInfo)
{
    const auto prepend = [this](juce::ValueTree newChild)
    {
        for (auto child : connectionHistory)
        {
            if (child.isEquivalentTo(newChild))
            {
                connectionHistory.removeChild(child, nullptr);
                break;
            }
        }

        while (connectionHistory.getNumChildren() >= 6)
        {
            connectionHistory.removeChild(connectionHistory.getChild(connectionHistory.getNumChildren() - 1), nullptr);
        }

        connectionHistory.addChild(newChild, 0, nullptr);
        file.create();
        file.replaceWithText(connectionHistory.toXmlString());
    };

    if (const auto* usbConnectionInfo = dynamic_cast<const ximu3::UsbConnectionInfo*>(&connectionInfo))
    {
        prepend({ "UsbConnectionInfo", {{ "portName", usbConnectionInfo->port_name }}});
        return;
    }

    if (const auto* serialConnectionInfo = dynamic_cast<const ximu3::SerialConnectionInfo*>(&connectionInfo))
    {
        prepend({ "SerialConnectionInfo", {{ "portName", serialConnectionInfo->port_name },
                                           { "baudRate", (int) serialConnectionInfo->baud_rate },
                                           { "rtsCtsEnabled", serialConnectionInfo->rts_cts_enabled ? 1 : 0 }}});
        return;
    }

    if (const auto* tcpConnectionInfo = dynamic_cast<const ximu3::TcpConnectionInfo*>(&connectionInfo))
    {
        prepend({ "TcpConnectionInfo", {{ "ipAddress", tcpConnectionInfo->ip_address },
                                        { "port", (int) tcpConnectionInfo->port }}});
        return;
    }

    if (const auto* udpConnectionInfo = dynamic_cast<const ximu3::UdpConnectionInfo*>(&connectionInfo))
    {
        prepend({ "UdpConnectionInfo", {{ "ipAddress", udpConnectionInfo->ip_address },
                                        { "sendPort", (int) udpConnectionInfo->send_port },
                                        { "receivePort", (int) udpConnectionInfo->receive_port }}});
        return;
    }

    if (const auto* bluetoothConnectionInfo = dynamic_cast<const ximu3::BluetoothConnectionInfo*>(&connectionInfo))
    {
        prepend({ "BluetoothConnectionInfo", {{ "portName", bluetoothConnectionInfo->port_name }}});
    }
}

std::vector<std::unique_ptr<ximu3::ConnectionInfo>> ConnectionHistory::get() const
{
    std::vector<std::unique_ptr<ximu3::ConnectionInfo>> result;

    for (auto connectionInfo : connectionHistory)
    {
        if (connectionInfo.hasType("UsbConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::UsbConnectionInfo>(connectionInfo["portName"].toString().toStdString()));
            continue;
        }

        if (connectionInfo.hasType("SerialConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::SerialConnectionInfo>(connectionInfo["portName"].toString().toStdString(),
                                                                           (uint32_t) (int) connectionInfo["baudRate"],
                                                                           connectionInfo["rtsCtsEnabled"]));
            continue;
        }

        if (connectionInfo.hasType("TcpConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::TcpConnectionInfo>(connectionInfo["ipAddress"].toString().toStdString(),
                                                                        (uint16_t) (int) connectionInfo["port"]));
            continue;
        }

        if (connectionInfo.hasType("UdpConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::UdpConnectionInfo>(connectionInfo["ipAddress"].toString().toStdString(),
                                                                        (uint16_t) (int) connectionInfo["sendPort"],
                                                                        (uint16_t) (int) connectionInfo["receivePort"]));
            continue;
        }

        if (connectionInfo.hasType("BluetoothConnectionInfo"))
        {
            result.push_back(std::make_unique<ximu3::BluetoothConnectionInfo>(connectionInfo["portName"].toString().toStdString()));
        }
    }

    return result;
}
