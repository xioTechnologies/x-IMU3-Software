#include "ConnectionPanel.h"
#include "ConnectionPanelContainer.h"
#include "ConnectionPanelHeader.h"
#include "Dialogs/SendingCommandDialog.h"

ConnectionPanelHeader::ConnectionPanelHeader(ConnectionPanel& connectionPanel_, ConnectionPanelContainer& connectionPanelContainer_)
    : connectionPanel(connectionPanel_),
      connectionPanelContainer(connectionPanelContainer_),
      connection(connectionPanel.getConnection())
{
    addAndMakeVisible(retryButton);
    addChildComponent(strobeButton);
    addAndMakeVisible(title);
    addAndMakeVisible(rssiIcon);
    addAndMakeVisible(batteryIcon);

    retryButton.onClick = [&]
    {
        if (onRetry)
        {
            onRetry();
        }
    };

    strobeButton.onClick = [&]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("strobe", {}), std::vector<ConnectionPanel*> { &connectionPanel }));
    };

    const auto addNetworkAnnouncementCallback = [&](const std::string& ipAddress)
    {
        networkAnnouncementCallbackId = networkAnnouncement->addCallback(networkAnnouncementCallback = [&, ipAddress](auto message)
        {
            if (std::string(message.ip_address) == ipAddress)
            {
                if (message.rssi != -1)
                {
                    rssiIcon.update(message.rssi);
                }
                batteryIcon.update(message.battery, message.charging_status);
            }
        });
    };

    const auto connectionInfo = connection->getInfo();
    if (const auto* const tcpConnectionInfo = dynamic_cast<const ximu3::XIMU3_TcpConnectionInfo*>(connectionInfo.get()))
    {
        addNetworkAnnouncementCallback(tcpConnectionInfo->ip_address);
    }
    if (const auto* const udpConnectionInfo = dynamic_cast<const ximu3::XIMU3_UdpConnectionInfo*>(connectionInfo.get()))
    {
        addNetworkAnnouncementCallback(udpConnectionInfo->ip_address);
    }

    rssiCallbackId = connectionPanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)
    {
        rssiIcon.update((int) message.percentage);
    });

    batteryCallbackId = connectionPanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)
    {
        batteryIcon.update((int) message.percentage, ximu3::XIMU3_charging_status_from_float(message.charging_status));
    });

    setMouseCursor(juce::MouseCursor::DraggingHandCursor);

    setState(State::connecting);
}

ConnectionPanelHeader::~ConnectionPanelHeader()
{
    if (networkAnnouncementCallbackId)
    {
        networkAnnouncement->removeCallback(*networkAnnouncementCallbackId);
    }

    connectionPanel.getConnection()->removeCallback(rssiCallbackId);
    connectionPanel.getConnection()->removeCallback(batteryCallbackId);

    *destroyed = true;
}

void ConnectionPanelHeader::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundLightest);
    g.setColour(connectionPanel.getTag());
    g.fillRect(getLocalBounds().removeFromLeft(UILayout::tagWidth));
}

void ConnectionPanelHeader::resized()
{
    static constexpr int margin = 7;

    auto bounds = getLocalBounds().withSizeKeepingCentre(getWidth() - 2 * margin, 20);

    bounds.removeFromLeft(UILayout::tagWidth);

    retryButton.setBounds(bounds.removeFromLeft(bounds.getHeight()));
    strobeButton.setBounds(retryButton.getBounds());
    bounds.removeFromLeft(margin);

    const auto showIconText = getWidth() > 500;

    batteryIcon.setBounds(bounds.removeFromRight(batteryIcon.getWidth(showIconText)));
    bounds.removeFromRight(margin);
    rssiIcon.setBounds(bounds.removeFromRight(rssiIcon.getWidth(showIconText)));
    bounds.removeFromRight(margin);

    title.setBounds(bounds);
}

void ConnectionPanelHeader::mouseDown(const juce::MouseEvent& mouseEvent)
{
    mouseDrag(mouseEvent);
}

void ConnectionPanelHeader::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    if (auto* componentUnderMouse = juce::Desktop::getInstance().findComponentAt(mouseEvent.getScreenPosition()))
    {
        if (auto* targetPanel = componentUnderMouse->findParentComponentOfClass<ConnectionPanel>())
        {
            connectionPanelContainer.showDragOverlayAtComponent(*targetPanel, DragOverlay::Side::all);
            return;
        }
    }
    connectionPanelContainer.hideDragOverlay();
}

void ConnectionPanelHeader::mouseUp(const juce::MouseEvent& mouseEvent)
{
    connectionPanelContainer.hideDragOverlay();
    if (auto* componentUnderMouse = juce::Desktop::getInstance().findComponentAt(mouseEvent.getScreenPosition()))
    {
        if (auto* targetPanel = componentUnderMouse->findParentComponentOfClass<ConnectionPanel>(); targetPanel != nullptr && targetPanel != &connectionPanel)
        {
            connectionPanelContainer.movePanel(connectionPanel, *targetPanel);
        }
    }

    if (mouseEvent.mouseWasClicked() && connectionPanelContainer.getLayout() == ConnectionPanelContainer::Layout::accordion)
    {
        connectionPanelContainer.setExpandedConnectionPanel((connectionPanelContainer.getExpandedConnectionPanel() == &connectionPanel) ? nullptr : &connectionPanel);
    }
}

void ConnectionPanelHeader::setState(const State state)
{
    switch (state)
    {
        case State::connecting:
            updateTitle("Connecting");
            retryButton.setEnabled(false);
            break;

        case State::connected:
            {
                updateTitle("Pinging");
                retryButton.setVisible(false);
                strobeButton.setVisible(true);

                connection->pingAsync([&, connection_ = connection, destroyed_ = destroyed](ximu3::XIMU3_PingResponse response)
                {
                    juce::MessageManager::callAsync([&, connection_, destroyed_, response]
                    {
                        if (*destroyed_)
                        {
                            return;
                        }

                        if (response.result == ximu3::XIMU3_ResultOk)
                        {
                            updateTitle(response.device_name, response.serial_number);
                            return;
                        }

                        setState(State::connected); // retry
                    });
                });
                break;
            }

        case State::connectionFailed:
            updateTitle("Connection Failed");
            retryButton.setEnabled(true);
            break;
    }
}

juce::String ConnectionPanelHeader::getTitle() const
{
    return title.getText();
}

void ConnectionPanelHeader::updateTitle(const std::vector<CommandMessage>& responses)
{
    for (const auto& response : responses)
    {
        if (response.error)
        {
            continue;
        }

        if (CommandMessage::normaliseKey(response.key) == CommandMessage::normaliseKey("device_name"))
        {
            updateTitle(response.getValue(), serialNumber);
        }
        else if (CommandMessage::normaliseKey(response.key) == CommandMessage::normaliseKey("serial_number"))
        {
            updateTitle(deviceName, response.getValue());
        }
    }
}

void ConnectionPanelHeader::updateTitle(const juce::String& deviceName_, const juce::String& serialNumber_)
{
    deviceName = deviceName_;
    serialNumber = serialNumber_;
    updateTitle(deviceName + " " + serialNumber);
}

void ConnectionPanelHeader::updateTitle(const juce::String& status)
{
    title.setText(status + "    " + connection->getInfo()->toString());
}
