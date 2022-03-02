#pragma once

#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"

class NotificationAndErrorMessagesDialog : public Dialog,
                                           private juce::TableListBoxModel
{
public:
    struct Message
    {
        bool isError;
        uint64_t timestamp;
        juce::String message;
        bool isUnread = true;
    };

    NotificationAndErrorMessagesDialog(std::vector<Message>& messages_, const std::function<void()>& onClear);

    void resized() override;

    void messagesChanged();

private:
    enum class ColumnIDs
    {
        type = 1,
        timestamp,
        message,
    };

    std::vector<Message>& messages;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotificationAndErrorMessagesDialog)
};
