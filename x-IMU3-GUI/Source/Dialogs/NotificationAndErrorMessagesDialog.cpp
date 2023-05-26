#include "../Widgets/Icon.h"
#include <BinaryData.h>
#include "NotificationAndErrorMessagesDialog.h"

juce::String NotificationAndErrorMessagesDialog::Message::getIcon() const
{
    switch (type)
    {
        case Message::Type::notification:
            return unread ? BinaryData::speech_white_svg : BinaryData::speech_grey_svg;
        case Message::Type::error:
            return unread ? BinaryData::warning_orange_svg : BinaryData::warning_grey_svg;
    }
    return {}; // avoid compiler warning
}

juce::String NotificationAndErrorMessagesDialog::Message::getTooltip() const
{
    switch (type)
    {
        case Message::Type::notification:
            return "Notification";
        case Message::Type::error:
            return "Error";
    }
    return {}; // avoid compiler warning
}

juce::Colour NotificationAndErrorMessagesDialog::Message::getColour() const
{
    switch (type)
    {
        case Message::Type::notification:
            return juce::Colours::white;
        case Message::Type::error:
            return UIColours::warning;
    }
    return {}; // avoid compiler warning
}

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
            return new Icon(notificationMessage.getIcon(), notificationMessage.getTooltip(), 0.6f);

        case ColumnID::timestamp:
        {
            auto* label = new SimpleLabel(juce::String(1E-6f * (float) notificationMessage.timestamp, 3));
            if (notificationMessage.unread == false)
            {
                label->setColour(juce::Label::textColourId, juce::Colours::grey);
            }
            return label;
        }

        case ColumnID::message:
        {
            auto* label = new SimpleLabel(notificationMessage.message);
            if (notificationMessage.unread == false)
            {
                label->setColour(juce::Label::textColourId, juce::Colours::grey);
            }
            return label;
        }

        default:
            return nullptr;
    }
}
