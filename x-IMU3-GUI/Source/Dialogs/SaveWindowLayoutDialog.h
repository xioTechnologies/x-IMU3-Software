#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "MenuStrip/WindowLayouts.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/SimpleLabel.h"

class SaveWindowLayoutDialog : public Dialog
{
public:
    SaveWindowLayoutDialog();

    void resized() override;

    juce::String getLayoutName() const;

private:
    SimpleLabel nameLabel { "Name:" };
    CustomTextEditor nameValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaveWindowLayoutDialog)
};
