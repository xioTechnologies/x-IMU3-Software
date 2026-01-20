#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/Icon.h"
#include "Widgets/IconAndText.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class ConnectionPanel;

class ConnectionPanelContainer;

class ConnectionPanelHeader : public juce::Component {
public:
    ConnectionPanelHeader(ConnectionPanel &connectionPanel_, ConnectionPanelContainer &connectionPanelContainer_);

    ~ConnectionPanelHeader() override;

    void paint(juce::Graphics &g) override;

    void resized() override;

    void mouseDown(const juce::MouseEvent &mouseEvent) override;

    void mouseDrag(const juce::MouseEvent &mouseEvent) override;

    void mouseUp(const juce::MouseEvent &mouseEvent) override;

    juce::String getHeading() const;

    void updateHeading(const std::vector<std::optional<ximu3::CommandMessage> > &responses);

    void updateHeading(const juce::String &deviceName_, const juce::String &serialNumber_);

    void updateHeading(const juce::String &descriptor_);

    juce::String getDescriptor() const;

    void showRetry(std::function<void()> onClick);

    void showLocate();

private:
    ConnectionPanel &connectionPanel;
    ConnectionPanelContainer &connectionPanelContainer;
    const std::shared_ptr<ximu3::Connection> connection;

    juce::String deviceName, serialNumber;
    const juce::String configString = connection->getConfig()->toString();
    juce::String descriptor;

    IconButton retryButton{BinaryData::refresh_svg, "Retry"};
    IconButton locateButton{BinaryData::location_svg, "Locate Device (Strobe LED)"};
    SimpleLabel headingLabel;
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
