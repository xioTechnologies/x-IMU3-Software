#include "ConnectionPanel.h"
#include "ConnectionPanelContainer.h"
#include "ConnectionPanelHeader.h"
#include "Dialogs/SendingCommandDialog.h"

ConnectionPanelHeader::ConnectionPanelHeader(ConnectionPanel& connectionPanel_, juce::ThreadPool& threadPool_, ConnectionPanelContainer& connectionPanelContainer_)
        : connectionPanel(connectionPanel_),
          threadPool(threadPool_),
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

    networkAnnouncementCallbackID = networkAnnouncement->addCallback(networkAnnouncementCallback = [&](auto message)
    {
        if (juce::String(message.serial_number) == serialNumber)
        {
            if (message.rssi != -1)
            {
                rssiIcon.update(message.rssi);
            }
            batteryIcon.update(message.battery, message.charging_status);
        }
    });

    rssiCallbackID = connectionPanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)
    {
        rssiIcon.update((int) message.percentage);
    });

    batteryCallbackID = connectionPanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)
    {
        batteryIcon.update((int) message.percentage, (ximu3::XIMU3_ChargingStatus) message.charging_status);
    });

    setMouseCursor(juce::MouseCursor::DraggingHandCursor);

    setState(State::connecting);
}

ConnectionPanelHeader::~ConnectionPanelHeader()
{
    networkAnnouncement->removeCallback(networkAnnouncementCallbackID);
    connectionPanel.getConnection()->removeCallback(rssiCallbackID);
    connectionPanel.getConnection()->removeCallback(batteryCallbackID);

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

            const std::function<juce::ThreadPoolJob::JobStatus()> job = [&, connection = connection, destroyed = destroyed]
            {
                if (*destroyed)
                {
                    return juce::ThreadPoolJob::jobHasFinished;
                }

                auto response = connection->ping();

                if (response.result == ximu3::XIMU3_ResultOk)
                {
                    juce::MessageManager::callAsync([&, connection, destroyed, response]
                                                    {
                                                        if (*destroyed)
                                                        {
                                                            return;
                                                        }

                                                        updateTitle(response.device_name, response.serial_number);
                                                    });
                    return juce::ThreadPoolJob::jobHasFinished;
                }

                return juce::ThreadPoolJob::jobNeedsRunningAgain;
            };
            threadPool.addJob(job);
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
        if (response.getError())
        {
            continue;
        }

        if (CommandMessage::normaliseKey(response.key) == CommandMessage::normaliseKey("deviceName"))
        {
            updateTitle(response.value, serialNumber);
        }
        else if (CommandMessage::normaliseKey(response.key) == CommandMessage::normaliseKey("serialNumber"))
        {
            updateTitle(deviceName, response.value);
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
