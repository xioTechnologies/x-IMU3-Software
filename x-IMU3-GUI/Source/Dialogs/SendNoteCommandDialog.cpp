#include "SendNoteCommandDialog.h"
#include "Widgets/PopupMenuHeader.h"

SendNoteCommandDialog::SendNoteCommandDialog(const juce::String& title) : Dialog(BinaryData::note_svg, title, "Send", "Cancel", &recentNotesButton, iconButtonWidth)
{
    addAndMakeVisible(label);
    addAndMakeVisible(value);
    addAndMakeVisible(recentNotesButton);

    recentNotes = juce::ValueTree::fromXml(file.loadFileAsString());
    if (!recentNotes.isValid())
    {
        recentNotes = juce::ValueTree("RecentNotes");
        recentNotes.appendChild({ "Note", {{ "note", "Hello World!" }}}, nullptr);
    }

    value.onTextChange = [&]
    {
        setOkButton(value.isEmpty() == false);
    };

    value.setText(recentNotes.getChild(0)["note"], juce::sendNotification);

    recentNotesButton.setWantsKeyboardFocus(false);

    setSize(600, calculateHeight(1));
}

void SendNoteCommandDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();
    label.setBounds(bounds.removeFromLeft(columnWidth));
    value.setBounds(bounds);
}

juce::String SendNoteCommandDialog::getNote()
{
    juce::ValueTree newNote { "Note", {{ "note", value.getText() }}};

    for (const auto note : recentNotes)
    {
        if (note.isEquivalentTo(newNote))
        {
            recentNotes.removeChild(note, nullptr);
            break;
        }
    }

    while (recentNotes.getNumChildren() >= 12)
    {
        recentNotes.removeChild(recentNotes.getChild(recentNotes.getNumChildren() - 1), nullptr);
    }

    recentNotes.addChild(newNote, 0, nullptr);
    file.replaceWithText(recentNotes.toXmlString());

    return value.getText();
}

juce::PopupMenu SendNoteCommandDialog::getRecentNotesMenu()
{
    juce::PopupMenu menu;
    for (const auto note : recentNotes)
    {
        menu.addItem(note["note"], [&, note]
        {
            value.setText(note["note"], juce::sendNotification);
        });
    }
    return menu;
}
