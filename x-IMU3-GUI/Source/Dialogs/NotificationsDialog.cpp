#include "../Helpers.h"
#include "NotificationsDialog.h"
#include "../Widgets/Icon.h"

NotificationsDialog::NotificationsDialog(std::vector<NotificationMessage>& notificationMessages_, const std::function<void()>& onClear) : Dialog(BinaryData::warning_white_svg, "Notifications", "OK", "", &clearAllButton, 80, true),
                                                                                                                                          notificationMessages(notificationMessages_)
{
    addAndMakeVisible(clearAllButton);
    clearAllButton.onClick = [&, onClear = onClear]
    {
        notificationMessages.clear();
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
    table.setColour(juce::TableListBox::backgroundColourId, UIColours::background);

    setSize(800, 480);
}

void NotificationsDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds(true);

    static constexpr int headerHeight = 30;
    bounds.removeFromTop(headerHeight);
    typeLabel.setBounds(table.getHeader().getColumnPosition(0).withHeight(headerHeight));
    timestampLabel.setBounds(table.getHeader().getColumnPosition(1).withHeight(headerHeight));
    messageLabel.setBounds(table.getHeader().getColumnPosition(2).withHeight(headerHeight));

    table.setBounds(bounds);
}

void NotificationsDialog::notificationMessagesChanged()
{
    table.updateContent();
}

int NotificationsDialog::getNumRows()
{
    return (int) notificationMessages.size();
}

juce::Component* NotificationsDialog::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    delete existingComponentToUpdate;

    const auto& notificationMessage = notificationMessages[notificationMessages.size() - 1 - (size_t) rowNumber];

    switch ((ColumnIDs) columnID)
    {
        case ColumnIDs::type:
            struct CustomIcon : juce::Component
            {
                CustomIcon(const juce::String& svg) : icon(svg, "")
                {
                    addAndMakeVisible(icon);
                }

                void resized() override
                {
                    icon.setBounds(getLocalBounds().reduced(0, 3));
                }

                Icon icon;
            };

            return new CustomIcon(notificationMessage.isError ?
                                  (notificationMessage.isUnread ? BinaryData::warning_orange_svg : BinaryData::warning_grey_svg) :
                                  (notificationMessage.isUnread ? BinaryData::speech_white_svg : BinaryData::speech_grey_svg));

        case ColumnIDs::timestamp:
        {
            auto* label = new SimpleLabel(Helpers::formatTimestamp(notificationMessage.timestamp));
            if (notificationMessage.isUnread == false)
            {
                label->setColour(juce::Label::textColourId, UIColours::grey);
            }
            return label;
        }

        case ColumnIDs::message:
        {
            auto* label = new SimpleLabel(notificationMessage.message);
            if (notificationMessage.isUnread == false)
            {
                label->setColour(juce::Label::textColourId, UIColours::grey);
            }
            return label;
        }

        default:
            return nullptr;
    }
}
