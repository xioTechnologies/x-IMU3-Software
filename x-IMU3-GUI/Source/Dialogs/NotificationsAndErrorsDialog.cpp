#include <BinaryData.h>
#include "ConnectionPanel/ConnectionPanel.h"
#include "NotificationsAndErrorsDialog.h"
#include "Widgets/Icon.h"

juce::String NotificationsAndErrorsDialog::Message::getIcon() const
{
    switch (type)
    {
        case Type::notification:
            return unread ? BinaryData::speech_white_svg : BinaryData::speech_grey_svg;
        case Type::error:
            return unread ? BinaryData::error_red_svg : BinaryData::error_grey_svg;
    }
    return {}; // avoid compiler warning
}

juce::String NotificationsAndErrorsDialog::Message::getTooltip() const
{
    switch (type)
    {
        case Type::notification:
            return "Notification";
        case Type::error:
            return "Error";
    }
    return {}; // avoid compiler warning
}

juce::Colour NotificationsAndErrorsDialog::Message::getColour() const
{
    switch (type)
    {
        case Type::notification:
            return UIColours::foreground;
        case Type::error:
            return UIColours::error;
    }
    return {}; // avoid compiler warning
}

NotificationsAndErrorsDialog::NotificationsAndErrorsDialog(std::vector<Message>& messages_, const std::function<void()>& onClear, const ConnectionPanel& connectionPanel)
        : Dialog(BinaryData::speech_white_svg, "Notifications and Errors from " + connectionPanel.getTitle(), "Close", "", &clearButton, 70, true, connectionPanel.getTag()),
          messages(messages_)
{
    addAndMakeVisible(clearButton);
    addAndMakeVisible(typeLabel);
    addAndMakeVisible(timestampLabel);
    addAndMakeVisible(messageLabel);
    addAndMakeVisible(table);

    clearButton.onClick = [&, onClear = onClear]
    {
        messages.clear();
        onClear();
    };

    table.getHeader().addColumn("", (int) ColumnID::type, 70, 70, 70);
    table.getHeader().addColumn("", (int) ColumnID::timestamp, 100, 100, 100);
    table.getHeader().addColumn("", (int) ColumnID::message, 1);
    table.getHeader().setStretchToFitActive(true);
    table.setHeaderHeight(0);
    table.getViewport()->setScrollBarsShown(true, false);
    table.setWantsKeyboardFocus(false);

    setSize(800, 480);
}

void NotificationsAndErrorsDialog::resized()
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

void NotificationsAndErrorsDialog::messagesChanged()
{
    table.updateContent();
}

int NotificationsAndErrorsDialog::getNumRows()
{
    return (int) messages.size();
}

juce::Component* NotificationsAndErrorsDialog::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    if (rowNumber >= (int) messages.size())
    {
        return existingComponentToUpdate; // index may exceed size on Windows if display scaling >100%
    }

    delete existingComponentToUpdate;

    const auto& message = messages[messages.size() - 1 - (size_t) rowNumber];

    switch ((ColumnID) columnID)
    {
        case ColumnID::type:
            return new Icon(message.getIcon(), message.getTooltip(), 0.6f);

        case ColumnID::timestamp:
        {
            auto* label = new SimpleLabel(juce::String(1E-6f * (float) message.timestamp, 3));
            if (message.unread == false)
            {
                label->setColour(juce::Label::textColourId, juce::Colours::grey);
            }
            return label;
        }

        case ColumnID::message:
        {
            auto* label = new SimpleLabel(message.message);
            if (message.unread == false)
            {
                label->setColour(juce::Label::textColourId, juce::Colours::grey);
            }
            return label;
        }

        default:
            return nullptr;
    }
}
