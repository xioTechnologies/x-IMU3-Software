#include <BinaryData.h>
#include "StartSdCardLoggingDialog.h"
#include "Widgets/PopupMenuHeader.h"

StartSdCardLoggingDialog::StartSdCardLoggingDialog() : Dialog(BinaryData::microSD_svg, "Start SD Card Logging", "Start", "Cancel", &useDeviceSettingsButton, 175) {
    addAndMakeVisible(label);
    addAndMakeVisible(value);
    addChildComponent(emptyValue);
    addAndMakeVisible(useDeviceSettingsButton);

    value.onTextChange = [&] {
        validate();
    };
    value.setText("Logged Data " + juce::Time::getCurrentTime().formatted("%Y-%m-%d %H-%M-%S"));

    emptyValue.setEnabled(false);

    useDeviceSettingsButton.onClick = [&] {
        value.setVisible(useDeviceSettingsButton.getToggleState() == false);
        emptyValue.setVisible(useDeviceSettingsButton.getToggleState());

        validate();
    };

    setSize(600, calculateHeight(1));
}

void StartSdCardLoggingDialog::resized() {
    Dialog::resized();

    auto bounds = getContentBounds().removeFromTop(UILayout::textComponentHeight);
    label.setBounds(bounds.removeFromLeft(columnWidth));
    value.setBounds(bounds);
    emptyValue.setBounds(bounds);
}

std::string StartSdCardLoggingDialog::getFileName() const {
    return useDeviceSettingsButton.getToggleState() ? "null" : ('"' + value.getText().toStdString() + '"');
}

void StartSdCardLoggingDialog::validate() {
    setOkButton(useDeviceSettingsButton.getToggleState() || value.getText().isNotEmpty());
}
