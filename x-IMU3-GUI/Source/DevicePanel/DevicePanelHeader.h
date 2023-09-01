#pragma once

#include "../CommandMessage.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Widgets/StatusIcon.h"
#include "Ximu3.hpp"

class DevicePanel;

class DevicePanelContainer;

class DevicePanelHeader : public juce::Component
{
public:
    enum class State
    {
        connecting,
        connected,
        connectionFailed,
    };

    DevicePanelHeader(DevicePanel& devicePanel_, DevicePanelContainer& devicePanelContainer_);

    ~DevicePanelHeader() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

    void mouseDrag(const juce::MouseEvent& mouseEvent) override;

    void mouseUp(const juce::MouseEvent& mouseEvent) override;

    void setState(const State state);

    juce::String getTitle() const;

    void updateTitle(const std::vector<CommandMessage>& responses);

    void updateTitle(const juce::String& deviceName_, const juce::String& serialNumber_);

    std::function<void()> onRetry;

private:
    DevicePanel& devicePanel;
    DevicePanelContainer& devicePanelContainer;
    const std::shared_ptr<ximu3::Connection> connection;

    std::shared_ptr<std::atomic<bool>> destroyed = std::make_shared<std::atomic<bool>>(false);

    juce::String deviceName, serialNumber;

    IconButton retryButton { BinaryData::refresh_svg, "Retry" };
    IconButton strobeButton { BinaryData::location_svg, "Locate Device (Strobe LED)" };
    SimpleLabel title;
    StatusIcon rssiIcon { BinaryData::wifi_unknown_svg, "Wi-Fi RSSI" };
    StatusIcon batteryIcon { BinaryData::battery_unknown_svg, "Battery Level" };

    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;
    std::function<void(ximu3::XIMU3_NetworkAnnouncementMessage)> networkAnnouncementCallback;
    uint64_t networkAnnouncementCallbackID;

    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;
    uint64_t rssiCallbackID;

    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;
    uint64_t batteryCallbackID;

    void updateTitle(const juce::String& status);

    void updateRssi(const int percentage);

    void updateBattery(const int percentage, const ximu3::XIMU3_ChargingStatus status);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanelHeader)
};
