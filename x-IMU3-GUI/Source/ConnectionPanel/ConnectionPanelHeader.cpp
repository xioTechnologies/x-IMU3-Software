#include "ConnectionPanel.h"
#include "ConnectionPanelContainer.h"
#include "ConnectionPanelHeader.h"
#include "Dialogs/SendingCommandDialog.h"

ConnectionPanelHeader::ConnectionPanelHeader(ConnectionPanel& connectionPanel_, ConnectionPanelContainer& connectionPanelContainer_)
    : connectionPanel(connectionPanel_),
      connectionPanelContainer(connectionPanelContainer_),
      connection(connectionPanel.getConnection())
{
    addAndMakeVisible(strobeButton);
    addAndMakeVisible(title);
    addAndMakeVisible(rssiIcon);
    addAndMakeVisible(batteryIcon);

    strobeButton.onClick = [&]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("strobe", {}), std::vector<ConnectionPanel*> { &connectionPanel }));
    };

    const auto addNetworkAnnouncementCallback = [&](const std::string& ipAddress)
    {
        networkAnnouncementCallbackID = networkAnnouncement->addCallback(networkAnnouncementCallback = [&, ipAddress](auto message)
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

    rssiCallbackID = connectionPanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)
    {
        rssiIcon.update((int) message.percentage);
    });

    batteryCallbackID = connectionPanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)
    {
        batteryIcon.update((int) message.percentage, ximu3::XIMU3_charging_status_from_float(message.charging_status));
    });

    setMouseCursor(juce::MouseCursor::DraggingHandCursor);

    updateTitle("Unknown Device");
}

ConnectionPanelHeader::~ConnectionPanelHeader()
{
    if (networkAnnouncementCallbackID)
    {
        networkAnnouncement->removeCallback(*networkAnnouncementCallbackID);
    }

    connectionPanel.getConnection()->removeCallback(rssiCallbackID);
    connectionPanel.getConnection()->removeCallback(batteryCallbackID);
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

    strobeButton.setBounds(bounds.removeFromLeft(bounds.getHeight()));
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

void ConnectionPanelHeader::updateTitle(const juce::String& deviceNameAndSerialNumber)
{
    title.setText(deviceNameAndSerialNumber + "    " + connection->getInfo()->toString());
}
