#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/SimpleLabel.h"

class ErrorDialog : public Dialog
{
public:
    explicit ErrorDialog(const juce::String& error);

    void resized() override;

private:
    SimpleLabel label { "Error" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ErrorDialog)
};
