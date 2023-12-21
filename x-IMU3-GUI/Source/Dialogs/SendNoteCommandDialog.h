#pragma once

#include "ApplicationSettings.h"
#include "CommandMessage.h"
#include "Dialog.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class SendNoteCommandDialog : public Dialog
{
public:
    SendNoteCommandDialog(const juce::String& title);

    void resized() override;

    juce::String getNote();

private:
    SimpleLabel label { "Note:" };
    CustomTextEditor value;

    IconButton recentNotesButton { BinaryData::history_svg, "Recent Notes", std::bind(&SendNoteCommandDialog::getRecentNotesMenu, this) };

    juce::ValueTree recentNotes;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Recent Notes.xml");

    juce::PopupMenu getRecentNotesMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendNoteCommandDialog)
};
