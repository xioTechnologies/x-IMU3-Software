#include "ConnectionPanel.h"
#include "ConnectionPanelContainer.h"
#include "ConnectionPanelHeader.h"
#include "Dialogs/SendingCommandDialog.h"

ConnectionPanelHeader::ConnectionPanelHeader(ConnectionPanel& connectionPanel_, ConnectionPanelContainer& connectionPanelContainer_)
    : connectionPanel(connectionPanel_),
      connectionPanelContainer(connectionPanelContainer_),
      connection(connectionPanel.getConnection())
{
    addChildComponent(retryButton);
    addChildComponent(locateButton);
    addAndMakeVisible(headingLabel);
    addAndMakeVisible(rssiIcon);
    addAndMakeVisible(batteryIcon);

    locateButton.onClick = [&]
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
}

ConnectionPanelHeader::~ConnectionPanelHeader()
{
    if (networkAnnouncementCallbackId)
    {
        networkAnnouncement->removeCallback(*networkAnnouncementCallbackId);
    }

    connectionPanel.getConnection()->removeCallback(rssiCallbackId);
    connectionPanel.getConnection()->removeCallback(batteryCallbackId);
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

    if (retryButton.isVisible())
    {
        retryButton.setBounds(bounds.removeFromLeft(bounds.getHeight()));
    }
    if (locateButton.isVisible())
    {
        locateButton.setBounds(bounds.removeFromLeft(bounds.getHeight()));
    }
    bounds.removeFromLeft(margin);

    const auto showIconText = getWidth() > 500;

    batteryIcon.setBounds(bounds.removeFromRight(batteryIcon.getWidth(showIconText)));
    bounds.removeFromRight(margin);
    rssiIcon.setBounds(bounds.removeFromRight(rssiIcon.getWidth(showIconText)));
    bounds.removeFromRight(margin);

    headingLabel.setBounds(bounds);
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

juce::String ConnectionPanelHeader::getHeading() const
{
    return headingLabel.getText();
}

void ConnectionPanelHeader::updateHeading(const std::vector<CommandMessage>& responses)
{
    for (const auto& response : responses)
    {
        if (response.error)
        {
            continue;
        }

        if (CommandMessage::normaliseKey(response.key) == CommandMessage::normaliseKey("device_name"))
        {
            updateHeading(response.getValue(), serialNumber);
        }
        else if (CommandMessage::normaliseKey(response.key) == CommandMessage::normaliseKey("serial_number"))
        {
            updateHeading(deviceName, response.getValue());
        }
    }
}

void ConnectionPanelHeader::updateHeading(const juce::String& deviceName_, const juce::String& serialNumber_)
{
    deviceName = deviceName_;
    serialNumber = serialNumber_;
    updateHeading(deviceName + " " + serialNumber);
}

void ConnectionPanelHeader::updateHeading(const juce::String& status)
{
    headingLabel.setText(status + "    " + connectionInfoString);
}

void ConnectionPanelHeader::showRetry(std::function<void()> onClick)
{
    retryButton.setVisible(true);
    retryButton.setEnabled(onClick != nullptr);
    retryButton.onClick = onClick;
    locateButton.setVisible(false);

    resized();
}

void ConnectionPanelHeader::showLocate()
{
    retryButton.setVisible(false);
    locateButton.setVisible(true);

    resized();
}
