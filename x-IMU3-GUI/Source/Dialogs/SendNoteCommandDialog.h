#pragma once

#include "ApplicationSettings.h"
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

    IconButton previousNotesButton { BinaryData::history_svg, "Previous Notes", std::bind(&SendNoteCommandDialog::getPreviousNotesMenu, this) };

    juce::ValueTree previousNotes;
    const juce::File file = ApplicationSettings::getDirectory().getChildFile("Previous Notes.xml");

    juce::PopupMenu getPreviousNotesMenu();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendNoteCommandDialog)
};
