#include "../Helpers.h"
#include "../Widgets/Icon.h"
#include "NotificationAndErrorMessagesDialog.h"

NotificationAndErrorMessagesDialog::NotificationAndErrorMessagesDialog(std::vector<Message>& messages_, const std::function<void()>& onClear)
        : Dialog(BinaryData::speech_white_svg, "Notification and Error Messages", "Close", "", &clearAllButton, 80, true),
          messages(messages_)
{
    addAndMakeVisible(clearAllButton);
    clearAllButton.onClick = [&, onClear = onClear]
    {
        messages.clear();
        onClear();
    };

    addAndMakeVisible(typeLabel);
    addAndMakeVisible(timestampLabel);
    addAndMakeVisible(messageLabel);

    addAndMakeVisible(table);
    table.getHeader().addColumn("", (int) ColumnIDs::type, 70, 70, 70);
    table.getHeader().addColumn("", (int) ColumnIDs::timestamp, 100, 100, 100);
    table.getHeader().addColumn("", (int) ColumnIDs::message, 1);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setColour(juce::TableListBox::backgroundColourId, UIColours::background);
    table.setWantsKeyboardFocus(false);

    setSize(800, 480);
}

void NotificationAndErrorMessagesDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds(true);

    static constexpr int headerHeight = 30;
    bounds.removeFromTop(headerHeight);
    typeLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::type - 1).withHeight(headerHeight));
    timestampLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::timestamp - 1).withHeight(headerHeight));
    messageLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnIDs::message - 1).withHeight(headerHeight));

    table.setBounds(bounds);
}

void NotificationAndErrorMessagesDialog::messagesChanged()
{
    table.updateContent();
}

int NotificationAndErrorMessagesDialog::getNumRows()
{
    return (int) messages.size();
}

juce::Component* NotificationAndErrorMessagesDialog::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    delete existingComponentToUpdate;

    const auto& notificationMessage = messages[messages.size() - 1 - (size_t) rowNumber];

    switch ((ColumnIDs) columnID)
    {
        case ColumnIDs::type:
            if (notificationMessage.isError)
            {
                return new Icon(notificationMessage.isUnread ? BinaryData::warning_orange_svg : BinaryData::warning_grey_svg, 0.6f, "Error");
            }

            return new Icon(notificationMessage.isUnread ? BinaryData::speech_white_svg : BinaryData::speech_grey_svg, 0.6f, "Notification");

        case ColumnIDs::timestamp:
        {
            auto* label = new SimpleLabel(Helpers::formatTimestamp(notificationMessage.timestamp));
            if (notificationMessage.isUnread == false)
            {
                label->setColour(juce::Label::textColourId, juce::Colours::grey);
            }
            return label;
        }

        case ColumnIDs::message:
        {
            auto* label = new SimpleLabel(notificationMessage.message);
            if (notificationMessage.isUnread == false)
            {
                label->setColour(juce::Label::textColourId, juce::Colours::grey);
            }
            return label;
        }

        default:
            return nullptr;
    }
}
