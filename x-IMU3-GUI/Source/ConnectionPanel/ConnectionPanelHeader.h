#pragma once

#include "CommandMessage.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/IconAndText.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class ConnectionPanel;

class ConnectionPanelContainer;

class ConnectionPanelHeader : public juce::Component
{
public:
    ConnectionPanelHeader(ConnectionPanel& connectionPanel_, ConnectionPanelContainer& connectionPanelContainer_);

    ~ConnectionPanelHeader() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

    void mouseDrag(const juce::MouseEvent& mouseEvent) override;

    void mouseUp(const juce::MouseEvent& mouseEvent) override;

    juce::String getTitle() const;

    void updateTitle(const std::vector<CommandMessage>& responses);

    void updateTitle(const juce::String& deviceName_, const juce::String& serialNumber_);

    void updateTitle(const juce::String& status);

private:
    ConnectionPanel& connectionPanel;
    ConnectionPanelContainer& connectionPanelContainer;
    const std::shared_ptr<ximu3::Connection> connection;

    std::shared_ptr<std::atomic<bool>> destroyed = std::make_shared<std::atomic<bool>>(false);

    juce::String deviceName, serialNumber;

    IconButton strobeButton { BinaryData::location_svg, "Locate Device (Strobe LED)" };
    SimpleLabel title;
    RssiIconAndText rssiIcon;
    BatteryIconAndText batteryIcon;

    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;
    std::function<void(ximu3::XIMU3_NetworkAnnouncementMessage)> networkAnnouncementCallback;
    std::optional<uint64_t> networkAnnouncementCallbackId;

    std::function<void(ximu3::XIMU3_RssiMessage)> rssiCallback;
    uint64_t rssiCallbackId;

    std::function<void(ximu3::XIMU3_BatteryMessage)> batteryCallback;
    uint64_t batteryCallbackId;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionPanelHeader)
};
