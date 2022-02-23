#pragma once

#include "Dialog.h"
#include "../Widgets/SimpleLabel.h"

class NotificationsDialog : public Dialog,
                            private juce::TableListBoxModel
{
public:
    struct NotificationMessage
    {
        bool isError;
        uint64_t timestamp;
        juce::String message;
        bool isUnread = true;
    };

    NotificationsDialog(std::vector<NotificationMessage>& notificationMessages_, const std::function<void()>& onClear);

    void resized() override;

    void notificationMessagesChanged();

private:
    enum class ColumnIDs
    {
        type = 1,
        timestamp = 2,
        message = 3
    };

    std::vector<NotificationMessage>& notificationMessages;

    DialogButton clearAllButton { "Clear All" };

    SimpleLabel typeLabel { "Type", UIFonts::defaultFont, juce::Justification::centred };
    SimpleLabel timestampLabel { "Timestamp" };
    SimpleLabel messageLabel { "Message" };
    juce::TableListBox table { "", this };

    int getNumRows() override;

    void paintRowBackground(juce::Graphics&, int, int, int, bool) override
    {
    }

    void paintCell(juce::Graphics&, int, int, int, int, bool) override
    {
    }

    juce::Component* refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotificationsDialog)
};
