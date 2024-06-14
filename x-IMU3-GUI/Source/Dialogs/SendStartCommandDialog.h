#pragma once

#include "Dialog.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/SimpleLabel.h"

class SendStartCommandDialog : public Dialog
{
public:
    SendStartCommandDialog(const juce::String& title);

    void resized() override;

    juce::String getFileName() const;

private:
    SimpleLabel label { "File Name:" };
    CustomTextEditor value;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendStartCommandDialog)
};
