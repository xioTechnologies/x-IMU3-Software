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
    table.getHeader().addColumn("", (int) ColumnID::type, 70, 70, 70);
    table.getHeader().addColumn("", (int) ColumnID::timestamp, 100, 100, 100);
    table.getHeader().addColumn("", (int) ColumnID::message, 1);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setColour(juce::TableListBox::backgroundColourId, UIColours::backgroundDark);
    table.setWantsKeyboardFocus(false);

    setSize(800, 480);
}

void NotificationAndErrorMessagesDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds(true);

    static constexpr int headerHeight = 30;
    bounds.removeFromTop(headerHeight);
    typeLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnID::type - 1).withHeight(headerHeight));
    timestampLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnID::timestamp - 1).withHeight(headerHeight));
    messageLabel.setBounds(table.getHeader().getColumnPosition((int) ColumnID::message - 1).withHeight(headerHeight));

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
    if (rowNumber >= (int) messages.size())
    {
        return existingComponentToUpdate; // index may exceed size on Windows if display scaling >100%
    }

    delete existingComponentToUpdate;

    const auto& notificationMessage = messages[messages.size() - 1 - (size_t) rowNumber];

    switch ((ColumnID) columnID)
    {
        case ColumnID::type:
            if (notificationMessage.isError)
            {
                return new Icon(notificationMessage.isUnread ? BinaryData::warning_orange_svg : BinaryData::warning_grey_svg, "Error", 0.6f);
            }

            return new Icon(notificationMessage.isUnread ? BinaryData::speech_white_svg : BinaryData::speech_grey_svg, "Notification", 0.6f);

        case ColumnID::timestamp:
        {
            auto* label = new SimpleLabel(Helpers::formatTimestamp(notificationMessage.timestamp));
            if (notificationMessage.isUnread == false)
            {
                label->setColour(juce::Label::textColourId, juce::Colours::grey);
            }
            return label;
        }

        case ColumnID::message:
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
