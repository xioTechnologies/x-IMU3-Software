#pragma once

#include "../Dialogs/NotificationsDialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Notifications.h"
#include "../Widgets/IconButton.h"
#include "../Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class DevicePanelFooter : public juce::Component
{
public:
    DevicePanelFooter(Notifications& notificationsPopup_, ximu3::Connection& connection_);

    ~DevicePanelFooter() override;

    void paint(juce::Graphics&) override;

    void resized() override;

private:
    Notifications& notificationsPopup;
    ximu3::Connection& connection;

    SimpleLabel statisticsLabel { "", UIFonts::smallFont };

    SimpleLabel latestNotificationMessage { "", UIFonts::smallFont, juce::Justification::centredRight };

    IconButton notificationsButton { IconButton::Style::normal, BinaryData::speech_grey_svg, 1.0f, "Notification Messages", nullptr, BinaryData::speech_white_svg };
    IconButton errorsButton { IconButton::Style::normal, BinaryData::warning_grey_svg, 1.0f, "Error Messages", nullptr, BinaryData::warning_orange_svg };
    SimpleLabel numberOfNotificationsLabel { "0", UIFonts::smallFont, juce::Justification::right };
    SimpleLabel numberOfErrorsLabel { "0", UIFonts::smallFont, juce::Justification::right };

    std::function<void(ximu3::XIMU3_NotificationMessage)> notificationCallback;
    uint64_t notificationCallbackID;
    std::function<void(ximu3::XIMU3_ErrorMessage)> errorCallback;
    uint64_t errorCallbackID;
    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;
    uint64_t statisticsCallbackID;

    std::vector<NotificationsDialog::NotificationMessage> notificationMessages;

    void notificationMessagesChanged(const bool showLatest);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanelFooter)
};
