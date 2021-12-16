#pragma once

#include "../CommandMessage.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class DevicePanel;

class DevicePanelContainer;

class DevicePanelHeader : public juce::Component,
                          private juce::Thread,
                          private juce::Timer
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

    void updateDeviceNameAndSerialNumber(const std::vector<CommandMessage>& responses);

    juce::String getDeviceNameAndSerialNumber() const;

private:
    DevicePanel& devicePanel;
    DevicePanelContainer& devicePanelContainer;

    juce::String deviceName, serialNumber;

    IconButton menuButton { IconButton::Style::normal, BinaryData::menu_svg, 1.0f, "Device Actions", std::bind(&DevicePanelHeader::getMenu, this) };
    SimpleLabel deviceNameAndSerialNumber;
    SimpleLabel connectionInfo;
    Icon rssiIcon { BinaryData::wifi_unknown_svg, "Wi-Fi RSSI" };
    Icon batteryIcon { BinaryData::battery_unknown_svg, "Battery Level" };
    SimpleLabel rssiText;
    SimpleLabel batteryText;

    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;
    uint64_t batteryCallbackID;

    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;
    uint64_t rssiCallbackID;

    std::atomic<int> batteryTimeout { std::numeric_limits<int>::max() };
    std::atomic<int> batteryValue { 0 };
    std::atomic<ximu3::XIMU3_ChargingStatus> chargingStatus { ximu3::XIMU3_ChargingStatus::XIMU3_ChargingStatusNotConnected };
    std::atomic<int> rssiTimeout { std::numeric_limits<int>::max() };
    std::atomic<int> rssiValue { 0 };

    juce::PopupMenu getMenu() const;

    void run() override;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanelHeader)
};
