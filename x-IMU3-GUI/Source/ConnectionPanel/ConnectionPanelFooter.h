#pragma once

#include "Dialogs/NotificationsAndErrorsDialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class ConnectionPanel;

class ConnectionPanelFooter : public juce::Component, private juce::Timer
{
public:
    ConnectionPanelFooter(ConnectionPanel& connectionPanel_);

    ~ConnectionPanelFooter() override;

    void paint(juce::Graphics&) override;

    void resized() override;

private:
    ConnectionPanel& connectionPanel;

    SimpleLabel statisticsLabel { "", UIFonts::getSmallFont() };

    SimpleLabel latestMessageLabel { "", UIFonts::getSmallFont(), juce::Justification::centredRight };

    IconButton notificationsButton { BinaryData::speech_grey_svg, "Notification Messages", nullptr, false, BinaryData::speech_white_svg };
    IconButton errorsButton { BinaryData::error_grey_svg, "Error Messages", nullptr, false, BinaryData::error_red_svg };
    SimpleLabel numberOfNotificationsLabel { "0", UIFonts::getSmallFont(), juce::Justification::right };
    SimpleLabel numberOfErrorsLabel { "0", UIFonts::getSmallFont(), juce::Justification::right };

    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;
    uint64_t statisticsCallbackID;
    std::function<void(ximu3::XIMU3_NotificationMessage)> notificationCallback;
    uint64_t notificationCallbackID;
    std::function<void(ximu3::XIMU3_ErrorMessage)> errorCallback;
    uint64_t errorCallbackID;

    std::vector<NotificationsAndErrorsDialog::Message> messages;

    void messagesChanged();

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionPanelFooter)
};
