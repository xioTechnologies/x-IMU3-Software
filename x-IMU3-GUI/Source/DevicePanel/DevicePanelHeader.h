#pragma once

#include "../CommandMessage.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/IconAndText.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class DevicePanel;

class DevicePanelContainer;

class DevicePanelHeader : public juce::Component
{
public:
    DevicePanelHeader(DevicePanel& devicePanel_, DevicePanelContainer& devicePanelContainer_);

    ~DevicePanelHeader() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

    void mouseDrag(const juce::MouseEvent& mouseEvent) override;

    void mouseUp(const juce::MouseEvent& mouseEvent) override;

    void updateDeviceDescriptor(const std::vector<CommandMessage>& responses);

    juce::String getDeviceDescriptor() const;

private:
    DevicePanel& devicePanel;
    DevicePanelContainer& devicePanelContainer;

    std::shared_ptr<std::atomic<bool>> pingInProgress = std::make_shared<std::atomic<bool>>(true);
    juce::String deviceName, serialNumber;

    IconButton strobeButton { BinaryData::brightness_svg, "Strobe LED" };
    SimpleLabel deviceDescriptor;
    SimpleLabel connectionInfo;
    IconAndText rssiIcon { BinaryData::wifi_unknown_svg, "Wi-Fi RSSI" };
    IconAndText batteryIcon { BinaryData::battery_unknown_svg, "Battery Level" };

    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;
    std::function<void(ximu3::XIMU3_NetworkAnnouncementMessage)> networkAnnouncementCallback;
    uint64_t networkAnnouncementCallbackID;

    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;
    uint64_t rssiCallbackID;

    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;
    uint64_t batteryCallbackID;

    void updateRssi(const int percentage);

    void updateBattery(const int percentage, const ximu3::XIMU3_ChargingStatus status);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanelHeader)
};
