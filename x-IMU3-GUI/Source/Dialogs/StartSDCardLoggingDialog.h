#pragma once

#include "../Widgets/CustomTextEditor.h"
#include "../Widgets/CustomToggleButton.h"
#include "../Widgets/SimpleLabel.h"
#include "Dialog.h"

class StartSdCardLoggingDialog : public Dialog {
public:
    StartSdCardLoggingDialog();

    void resized() override;

    std::string getFileName() const;

private:
    SimpleLabel label{"File Name:"};
    CustomTextEditor value;
    CustomTextEditor emptyValue;

    CustomToggleButton useDeviceSettingsButton{"Use Device Settings"};

    void validate();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StartSdCardLoggingDialog)
};
