#pragma once

#include "../Dialogs/NotificationAndErrorMessagesDialog.h"
#include "../Widgets/IconButton.h"
#include "../Widgets/SimpleLabel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Ximu3.hpp"

class DevicePanelFooter : public juce::Component
{
public:
    DevicePanelFooter(ximu3::Connection& connection_);

    ~DevicePanelFooter() override;

    void paint(juce::Graphics&) override;

    void resized() override;

private:
    ximu3::Connection& connection;

    SimpleLabel statisticsLabel { "", UIFonts::getSmallFont() };

    SimpleLabel latestMessageLabel { "", UIFonts::getSmallFont(), juce::Justification::centredRight };

    IconButton notificationsButton { BinaryData::speech_grey_svg, "Notification Messages", nullptr, false, BinaryData::speech_white_svg };
    IconButton errorsButton { BinaryData::warning_grey_svg, "Error Messages", nullptr, false, BinaryData::warning_orange_svg };
    SimpleLabel numberOfNotificationsLabel { "0", UIFonts::getSmallFont(), juce::Justification::right };
    SimpleLabel numberOfErrorsLabel { "0", UIFonts::getSmallFont(), juce::Justification::right };

    std::function<void(ximu3::XIMU3_Statistics)> statisticsCallback;
    uint64_t statisticsCallbackID;
    std::function<void(ximu3::XIMU3_NotificationMessage)> notificationCallback;
    uint64_t notificationCallbackID;
    std::function<void(ximu3::XIMU3_ErrorMessage)> errorCallback;
    uint64_t errorCallbackID;

    std::vector<NotificationAndErrorMessagesDialog::Message> messages;

    void messagesChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanelFooter)
};
