#include "SendNoteCommandDialog.h"
#include "Widgets/PopupMenuHeader.h"

SendNoteCommandDialog::SendNoteCommandDialog(const juce::String& title) : Dialog(BinaryData::note_svg, title, "Send", "Cancel", &previousNotesButton, iconButtonWidth)
{
    addAndMakeVisible(label);
    addAndMakeVisible(value);
    addAndMakeVisible(previousNotesButton);

    previousNotes = juce::ValueTree::fromXml(file.loadFileAsString());
    if (!previousNotes.isValid())
    {
        previousNotes = juce::ValueTree("Notes");
        previousNotes.appendChild({ "Note", { { "note", "This message will be echoed as a timestamped notification" } } }, nullptr);
    }

    value.onTextChange = [&]
    {
        setOkButton(value.isEmpty() == false);
    };

    value.setText(previousNotes.getChild(0)["note"], juce::sendNotification);

    previousNotesButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(1));
}

void SendNoteCommandDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds().removeFromTop(UILayout::textComponentHeight);
    label.setBounds(bounds.removeFromLeft(columnWidth));
    value.setBounds(bounds);
}

juce::String SendNoteCommandDialog::getNote()
{
    juce::ValueTree newNote { "Note", { { "note", value.getText() } } };

    for (const auto note : previousNotes)
    {
        if (note.isEquivalentTo(newNote))
        {
            previousNotes.removeChild(note, nullptr);
            break;
        }
    }

    while (previousNotes.getNumChildren() >= 18)
    {
        previousNotes.removeChild(previousNotes.getChild(previousNotes.getNumChildren() - 1), nullptr);
    }

    previousNotes.addChild(newNote, 0, nullptr);
    file.replaceWithText(previousNotes.toXmlString());

    return value.getText();
}

juce::PopupMenu SendNoteCommandDialog::getPreviousNotesMenu()
{
    juce::PopupMenu menu;
    for (const auto note : previousNotes)
    {
        menu.addItem(note["note"], [&, note]
        {
            value.setText(note["note"], juce::sendNotification);
        });
    }
    return menu;
}
