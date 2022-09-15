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
    static constexpr int colourTagWidth = 4;
    static constexpr int margin = 10;

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

    juce::String deviceName, serialNumber;

    IconButton menuButton { IconButton::Style::normal, BinaryData::menu_svg, 1.0f, "Device Menu", std::bind(&DevicePanelHeader::getMenu, this) };
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

    void updateRssi(const uint32_t percentage);

    void updateBattery(const uint32_t percentage, const ximu3::XIMU3_ChargingStatus status);

    juce::PopupMenu getMenu() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanelHeader)
};
