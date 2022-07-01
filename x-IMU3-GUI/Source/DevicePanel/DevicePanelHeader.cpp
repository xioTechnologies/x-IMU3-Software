#include "../Dialogs/LedColourDialog.h"
#include "../Dialogs/SendCommandDialog.h"
#include "../Dialogs/SendingCommandDialog.h"
#include "DevicePanel.h"
#include "DevicePanelContainer.h"
#include "DevicePanelHeader.h"

DevicePanelHeader::DevicePanelHeader(DevicePanel& devicePanel_, DevicePanelContainer& devicePanelContainer_)
        : juce::Thread(""),
          devicePanel(devicePanel_),
          devicePanelContainer(devicePanelContainer_),
          connectionInfo(devicePanel.getConnection().getInfo()->toString(), UIFonts::defaultFont)
{
    addAndMakeVisible(menuButton);
    addAndMakeVisible(rssiIcon);
    addAndMakeVisible(batteryIcon);
    addAndMakeVisible(deviceDescriptor);
    addAndMakeVisible(connectionInfo);
    addAndMakeVisible(rssiText);
    addAndMakeVisible(batteryText);

    setMouseCursor(juce::MouseCursor::DraggingHandCursor);

    deviceDescriptor.setText(getDeviceDescriptor());

    batteryCallbackID = devicePanel.getConnection().addBatteryCallback(batteryCallback = [&](auto message)
    {
        batteryTimeout = 0;
        batteryValue = (int) message.percentage;
        chargingStatus = (ximu3::XIMU3_ChargingStatus) message.charging_status;
    });

    rssiCallbackID = devicePanel.getConnection().addRssiCallback(rssiCallback = [&](auto message)
    {
        rssiTimeout = 0;
        rssiValue = (int) message.percentage;
    });

    startThread();

    startTimerHz(10);
}

DevicePanelHeader::~DevicePanelHeader()
{
    devicePanel.getConnection().removeCallback(batteryCallbackID);
    devicePanel.getConnection().removeCallback(rssiCallbackID);

    stopThread(5000); // allow some time for ping to complete
}

void DevicePanelHeader::paint(juce::Graphics& g)
{
    g.fillAll(UIColours::menuStrip);
    g.setColour(devicePanel.getColourTag());
    g.fillRect(getLocalBounds().removeFromLeft(colourTagWidth));
}

void DevicePanelHeader::resized()
{
    auto bounds = getLocalBounds().withSizeKeepingCentre(getWidth() - 2 * margin, 20).toFloat();

    menuButton.setBounds(bounds.removeFromLeft(bounds.getHeight()).toNearestInt());
    bounds.removeFromLeft(margin);

    static constexpr int iconWidth = 18;
    static constexpr int textWidth = 45;

    juce::FlexBox flexBox;
    flexBox.items.add(juce::FlexItem(rssiIcon).withMinWidth(iconWidth).withMaxWidth(iconWidth));
    flexBox.items.add(juce::FlexItem(rssiText).withWidth(textWidth).withFlex(1.0f).withMargin({ 0.0f, 0.0f, 0.0f, (float) UILayout::panelMargin }));
    flexBox.items.add(juce::FlexItem(margin, bounds.getHeight()).withFlex(1.0f).withMinWidth(margin));
    flexBox.items.add(juce::FlexItem(batteryIcon).withMinWidth(iconWidth).withMaxWidth(iconWidth));
    flexBox.items.add(juce::FlexItem(batteryText).withWidth(textWidth).withFlex(1.0f).withMargin({ 0.0f, 0.0f, 0.0f, (float) UILayout::panelMargin }));
    flexBox.performLayout(bounds.removeFromRight(juce::jlimit((float) iconWidth + margin + iconWidth, (float) (textWidth + UILayout::panelMargin + iconWidth + margin + textWidth + UILayout::panelMargin + iconWidth), bounds.getWidth() - deviceDescriptor.getTextWidth() - margin - connectionInfo.getTextWidth() - margin)));

    bounds.removeFromRight(margin);
    deviceDescriptor.setBounds(bounds.removeFromLeft(deviceDescriptor.getTextWidth()).getSmallestIntegerContainer());
    bounds.removeFromLeft(margin);
    connectionInfo.setBounds(bounds.removeFromLeft(connectionInfo.getTextWidth()).getSmallestIntegerContainer());
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

    if (mouseEvent.mouseWasClicked())
    {
        devicePanelContainer.toggleAccordionState(&devicePanel);
    }
}

void DevicePanelHeader::updateDeviceDescriptor(const std::vector<CommandMessage>& responses)
{
    for (const auto& response : responses)
    {
        if (response.key == "deviceName")
        {
            deviceName = response.value;
        }
        else if (response.key == "serialNumber")
        {
            serialNumber = response.value;
        }
    }

    deviceDescriptor.setText(getDeviceDescriptor());
    resized();
}

juce::String DevicePanelHeader::getDeviceDescriptor() const
{
    if (deviceName.isEmpty() && serialNumber.isEmpty())
    {
        return "Unknown Device";
    }

    if (deviceName.isNotEmpty() && serialNumber.isNotEmpty())
    {
        return deviceName + " - " + serialNumber;
    }

    return deviceName + serialNumber;
}

juce::PopupMenu DevicePanelHeader::getMenu() const
{
    juce::PopupMenu menu;

    menu.addItem("Disconnect", [this]
    {
        devicePanelContainer.removePanel(devicePanel);
    });

    menu.addItem("Send Command", [this]
    {
        DialogLauncher::launchDialog(std::make_unique<SendCommandDialog>("Send Command to " + deviceDescriptor.getText()), [this]
        {
            if (auto* dialog = dynamic_cast<SendCommandDialog*>(DialogLauncher::getLaunchedDialog()))
            {
                DialogLauncher::launchDialog(std::make_unique<SendingCommandDialog>(CommandMessage(dialog->getCommand()), std::vector<DevicePanel*> { &devicePanel }));
            }
        });
    });

    menu.addItem("Strobe LED", [this]
    {
        DialogLauncher::launchDialog(std::make_unique<SendingCommandDialog>(CommandMessage("strobe", {}), std::vector<DevicePanel*> { &devicePanel }));
    });

    menu.addItem("LED Colour", [this]
    {
        DialogLauncher::launchDialog(std::make_unique<LedColourDialog>(devicePanel));
    });

    return menu;
}

void DevicePanelHeader::run()
{
    const auto response = devicePanel.getConnection().ping();

    juce::MessageManagerLock lock(this);
    if (lock.lockWasGained() == false)
    {
        return;
    }

    deviceName = response.device_name;
    serialNumber = response.serial_number;
    deviceDescriptor.setText(getDeviceDescriptor());
    resized();
}

void DevicePanelHeader::timerCallback()
{
    for (const auto& device : NetworkDiscoveryDispatcher::getSingleton().getDevices())
    {
        if (juce::String(device.serial_number) == serialNumber)
        {
            batteryTimeout = 0;
            batteryValue = device.battery;
            chargingStatus = device.status;

            rssiTimeout = 0;
            rssiValue = device.rssi;

            break;
        }
    }

    static constexpr int timeout = 5000;

    if (batteryTimeout < timeout)
    {
        batteryTimeout += getTimerInterval();

        batteryIcon.setIcon(chargingStatus == ximu3::XIMU3_ChargingStatusCharging ? BinaryData::battery_charging_svg :
                            chargingStatus == ximu3::XIMU3_ChargingStatusChargingComplete ? BinaryData::battery_charging_complete_svg :
                            batteryValue <= 25 ? BinaryData::battery_25_svg :
                            batteryValue <= 50 ? BinaryData::battery_50_svg :
                            batteryValue <= 75 ? BinaryData::battery_75_svg :
                            BinaryData::battery_100_svg);

        if (chargingStatus != ximu3::XIMU3_ChargingStatusNotConnected)
        {
            batteryText.setText("USB");
        }
        else
        {
            batteryText.setText(juce::String(batteryValue.load()) + "%");
        }
    }
    else
    {
        batteryIcon.setIcon(BinaryData::battery_unknown_svg);
        batteryText.setText("");
    }

    if (rssiTimeout < timeout)
    {
        rssiTimeout += getTimerInterval();

        rssiIcon.setIcon(rssiValue <= 25 ? BinaryData::wifi_25_svg :
                         rssiValue <= 50 ? BinaryData::wifi_50_svg :
                         rssiValue <= 75 ? BinaryData::wifi_75_svg :
                         BinaryData::wifi_100_svg);

        rssiText.setText(juce::String(rssiValue.load()) + "%");
    }
    else
    {
        rssiIcon.setIcon(BinaryData::wifi_unknown_svg);
        rssiText.setText("");
    }
}
