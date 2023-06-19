#include "../Dialogs/SendingCommandDialog.h"
#include "DevicePanel.h"
#include "DevicePanelContainer.h"
#include "DevicePanelHeader.h"

DevicePanelHeader::DevicePanelHeader(DevicePanel& devicePanel_, DevicePanelContainer& devicePanelContainer_)
        : devicePanel(devicePanel_),
          devicePanelContainer(devicePanelContainer_),
          connectionInfo(devicePanel.getConnection()->getInfo()->toString(), UIFonts::getDefaultFont())
{
    addAndMakeVisible(strobeButton);
    addAndMakeVisible(rssiIcon);
    addAndMakeVisible(batteryIcon);
    addAndMakeVisible(deviceDescriptor);
    addAndMakeVisible(connectionInfo);

    strobeButton.onClick = [&]
    {
        DialogQueue::getSingleton().pushFront(std::make_unique<SendingCommandDialog>(CommandMessage("strobe", {}), std::vector<DevicePanel*> { &devicePanel }));
    };

    deviceDescriptor.setText(getDeviceDescriptor());

    networkAnnouncementCallbackID = networkAnnouncement->addCallback(networkAnnouncementCallback = [&](auto message)
    {
        if (juce::String(message.serial_number) == serialNumber)
        {
            updateRssi(message.rssi);
            updateBattery(message.battery, message.charging_status);
        }
    });

    rssiCallbackID = devicePanel.getConnection()->addRssiCallback(rssiCallback = [&](auto message)
    {
        updateRssi((int) message.percentage);
    });

    batteryCallbackID = devicePanel.getConnection()->addBatteryCallback(batteryCallback = [&](auto message)
    {
        updateBattery((int) message.percentage, (ximu3::XIMU3_ChargingStatus) message.charging_status);
    });

    juce::Thread::launch([&, connection = devicePanel.getConnection(), pingInProgress = pingInProgress]
                         {
                             while (*pingInProgress)
                             {
                                 auto response = connection->ping();

                                 if (response.result == ximu3::XIMU3_ResultOk)
                                 {
                                     juce::MessageManager::callAsync([&, pingInProgress, response]
                                                                     {
                                                                         if (pingInProgress->exchange(false) == false)
                                                                         {
                                                                             return;
                                                                         }

                                                                         deviceName = response.device_name;
                                                                         serialNumber = response.serial_number;
                                                                         deviceDescriptor.setText(getDeviceDescriptor());
                                                                         resized();
                                                                     });
                                     return;
                                 }
                             }
                         });

    setMouseCursor(juce::MouseCursor::DraggingHandCursor);
}

DevicePanelHeader::~DevicePanelHeader()
{
    networkAnnouncement->removeCallback(networkAnnouncementCallbackID);
    devicePanel.getConnection()->removeCallback(rssiCallbackID);
    devicePanel.getConnection()->removeCallback(batteryCallbackID);

    *pingInProgress = false;
}

void DevicePanelHeader::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::backgroundLightest);
    g.setColour(devicePanel.getTag());
    g.fillRect(getLocalBounds().removeFromLeft(UILayout::tagWidth));
}

void DevicePanelHeader::resized()
{
    static constexpr int margin = 7;

    auto bounds = getLocalBounds().withSizeKeepingCentre(getWidth() - 2 * margin, 20);

    bounds.removeFromLeft(UILayout::tagWidth);

    strobeButton.setBounds(bounds.removeFromLeft(bounds.getHeight()));
    bounds.removeFromLeft(margin);

    const auto deviceDescriptorWidth = (int) std::ceil(deviceDescriptor.getTextWidth());
    const auto connectionInfoWidth = (int) std::ceil(connectionInfo.getTextWidth());
    const auto iconWidth = juce::jlimit(IconAndText::minimumWidth, IconAndText::maximumWidth, (int) (bounds.getWidth() - deviceDescriptorWidth - connectionInfoWidth - 3 * margin) / 2);

    batteryIcon.setBounds(bounds.removeFromRight(iconWidth));
    bounds.removeFromRight(margin);
    rssiIcon.setBounds(bounds.removeFromRight(iconWidth));
    bounds.removeFromRight(margin);

    deviceDescriptor.setBounds(bounds.removeFromLeft(deviceDescriptorWidth));
    bounds.removeFromLeft(margin);
    connectionInfo.setBounds(bounds.removeFromLeft(connectionInfoWidth));
}

void DevicePanelHeader::mouseDown(const juce::MouseEvent& mouseEvent)
{
    mouseDrag(mouseEvent);
}

void DevicePanelHeader::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    if (auto* componentUnderMouse = juce::Desktop::getInstance().findComponentAt(mouseEvent.getScreenPosition()))
    {
        if (auto* targetPanel = componentUnderMouse->findParentComponentOfClass<DevicePanel>())
        {
            devicePanelContainer.showDragOverlayAtComponent(*targetPanel, DragOverlay::Side::all);
            return;
        }
    }
    devicePanelContainer.hideDragOverlay();
}

void DevicePanelHeader::mouseUp(const juce::MouseEvent& mouseEvent)
{
    devicePanelContainer.hideDragOverlay();
    if (auto* componentUnderMouse = juce::Desktop::getInstance().findComponentAt(mouseEvent.getScreenPosition()))
    {
        if (auto* targetPanel = componentUnderMouse->findParentComponentOfClass<DevicePanel>(); targetPanel != nullptr && targetPanel != &devicePanel)
        {
            devicePanelContainer.movePanel(devicePanel, *targetPanel);
        }
    }

    if (mouseEvent.mouseWasClicked() && devicePanelContainer.getLayout() == DevicePanelContainer::Layout::accordion)
    {
        devicePanelContainer.setExpandedDevicePanel((devicePanelContainer.getExpandedDevicePanel() == &devicePanel) ? nullptr : &devicePanel);
    }
}

void DevicePanelHeader::updateDeviceDescriptor(const std::vector<CommandMessage>& responses)
{
    for (const auto& response : responses)
    {
        if (CommandMessage::normaliseKey(response.key) == CommandMessage::normaliseKey("deviceName"))
        {
            deviceName = response.value;
        }
        else if (CommandMessage::normaliseKey(response.key) == CommandMessage::normaliseKey("serialNumber"))
        {
            serialNumber = response.value;
        }
    }

    deviceDescriptor.setText(getDeviceDescriptor());
    resized();
}

juce::String DevicePanelHeader::getDeviceDescriptor() const
{
    if (*pingInProgress)
    {
        return "Unknown Device";
    }
    return deviceName + " - " + serialNumber;
}

void DevicePanelHeader::updateRssi(const int percentage)
{
    if (percentage < 0)
    {
        return;
    }

    rssiIcon.update(percentage <= 25 ? BinaryData::wifi_25_svg :
                    percentage <= 50 ? BinaryData::wifi_50_svg :
                    percentage <= 75 ? BinaryData::wifi_75_svg :
                    BinaryData::wifi_100_svg,
                    juce::String(percentage) + "%");
}

void DevicePanelHeader::updateBattery(const int percentage, const ximu3::XIMU3_ChargingStatus status)
{
    batteryIcon.update(status == ximu3::XIMU3_ChargingStatusCharging ? BinaryData::battery_charging_svg :
                       status == ximu3::XIMU3_ChargingStatusChargingComplete ? BinaryData::battery_charging_complete_svg :
                       percentage <= 25 ? BinaryData::battery_25_svg :
                       percentage <= 50 ? BinaryData::battery_50_svg :
                       percentage <= 75 ? BinaryData::battery_75_svg :
                       BinaryData::battery_100_svg,
                       status != ximu3::XIMU3_ChargingStatusNotConnected ? "USB" : juce::String(percentage) + "%");
}
