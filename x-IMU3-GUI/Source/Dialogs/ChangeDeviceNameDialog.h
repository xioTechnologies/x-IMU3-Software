#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomTextEditor.h"
#include "Widgets/SimpleLabel.h"

class ChangeDeviceNameDialog : public Dialog
{
public:
    explicit ChangeDeviceNameDialog(const juce::String& defaultDeviceName);

    void resized() override;

    juce::String getDeviceName() const;

private:
    SimpleLabel nameLabel { "Name:" };
    CustomTextEditor nameValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeDeviceNameDialog)
};
