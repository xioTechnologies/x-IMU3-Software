#pragma once

#include "Dialog.h"

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

    NotificationsDialog(std::vector<NotificationMessage>& notificationMessages_);

    void resized() override;

    void notificationMessagesChanged();

    std::function<void()> onClear;

private:
    enum class ColumnIDs
    {
        type = 1,
        timestamp = 2,
        message = 3
    };

    std::vector<NotificationMessage>& notificationMessages;

    juce::TableListBox table { "", this };

    static bool isScrolledToBottom(const juce::TableListBox&);

    int getNumRows() override;

    void paintRowBackground(juce::Graphics& g, int rowNumber, int, int, bool) override;

    void paintCell(juce::Graphics&, int, int, int, int, bool) override
    {
    }

    juce::Component* refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate) override;

    void cellClicked(int, int, const juce::MouseEvent& mouseEvent) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotificationsDialog)
};
