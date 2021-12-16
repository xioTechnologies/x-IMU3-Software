#pragma once

#include "../Widgets/CustomToggleButton.h"
#include "Dialog.h"

class ApplicationErrorsDialog : public Dialog
{
public:
    ApplicationErrorsDialog();

    ~ApplicationErrorsDialog() override;

    void resized() override;

    static void addError(const juce::String& message);

    static juce::Value numberOfUnreadErrors;

private:
    CustomToggleButton showErrorsButton { "Show Application Errors" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApplicationErrorsDialog)
};
