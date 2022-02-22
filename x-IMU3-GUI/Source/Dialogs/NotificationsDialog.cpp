#include "../Helpers.h"
#include "NotificationsDialog.h"
#include "../Widgets/Icon.h"
#include "../Widgets/SimpleLabel.h"

NotificationsDialog::NotificationsDialog(std::vector<NotificationMessage>& notificationMessages_) : Dialog(BinaryData::warning_white_svg, "Notifications", "OK", "", nullptr, 0, true), notificationMessages(notificationMessages_)
{
    addAndMakeVisible(table);
    table.getHeader().addColumn("Type", (int) ColumnIDs::type, 100, 100, 100);
    table.getHeader().addColumn("Timestamp", (int) ColumnIDs::timestamp, 100, 100, 100);
    table.getHeader().addColumn("Message", (int) ColumnIDs::message, 1);
    table.getHeader().setStretchToFitActive(true);

    setSize(800, 480);
}

void NotificationsDialog::resized()
{
    Dialog::resized();

    const auto wasScrolledToBottom = isScrolledToBottom(table);
    table.setBounds(getContentBounds());
    if (wasScrolledToBottom)
    {
        table.getVerticalScrollBar().scrollToBottom(juce::sendNotificationSync);
    }
}

void NotificationsDialog::notificationMessagesChanged()
{
    const auto wasScrolledToBottom = isScrolledToBottom(table);
    table.updateContent();
    if (wasScrolledToBottom)
    {
        table.getVerticalScrollBar().scrollToBottom(juce::sendNotificationSync);
    }
}

bool NotificationsDialog::isScrolledToBottom(const juce::TableListBox& table)
{
    return (table.getVerticalScrollBar().isVisible() == false) || (table.getVerticalScrollBar().getCurrentRange().getEnd() == table.getVerticalScrollBar().getMaximumRangeLimit());
}

int NotificationsDialog::getNumRows()
{
    return (int) notificationMessages.size();
}

void NotificationsDialog::paintRowBackground(juce::Graphics& g, int rowNumber, int, int, bool)
{
    g.fillAll(rowNumber % 2 == 0 ? UIColours::menuStrip : UIColours::background);
}

juce::Component* NotificationsDialog::refreshComponentForCell(int rowNumber, int columnID, bool, juce::Component* existingComponentToUpdate)
{
    delete existingComponentToUpdate;

    const auto& notificationMessage = notificationMessages[(size_t) rowNumber];

    switch ((ColumnIDs) columnID)
    {
        case ColumnIDs::type:
            struct PaddedIcon : juce::Component
            {
                PaddedIcon(const juce::String& svg) : icon(svg, "")
                {
                    addAndMakeVisible(icon);
                }

                void resized() override
                {
                    icon.setBounds(getLocalBounds().reduced(0, 3));
                }

                Icon icon;
            };

            return new PaddedIcon(notificationMessage.isError ? BinaryData::warning_orange_svg : BinaryData::speech_white_svg);

        case ColumnIDs::timestamp:
            return new SimpleLabel(Helpers::formatTimestamp(notificationMessage.timestamp));

        case ColumnIDs::message:
            return new SimpleLabel(notificationMessage.message);

        default:
            return nullptr;
    }
}

void NotificationsDialog::cellClicked(int, int, const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.mods.isPopupMenu())
    {
        juce::PopupMenu menu;
        menu.addItem("Copy To Clipboard", [this]
        {
            juce::String text;
            for (const auto& notificationMessage : notificationMessages)
            {
                text += Helpers::formatTimestamp(notificationMessage.timestamp) + " " + notificationMessage.message + "\n";
            }
            juce::SystemClipboard::copyTextToClipboard(text);
        });
        menu.addItem("Clear All", [this]
        {
            notificationMessages.clear();
            table.updateContent();
            if (onClear != nullptr)
            {
                onClear();
            }
        });
        menu.showAt({ mouseEvent.getMouseDownScreenX(), mouseEvent.getMouseDownScreenY() - 10, 10, 10 });
    }
}
