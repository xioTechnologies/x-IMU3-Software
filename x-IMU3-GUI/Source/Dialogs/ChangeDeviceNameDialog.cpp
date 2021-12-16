#include "ChangeDeviceNameDialog.h"

ChangeDeviceNameDialog::ChangeDeviceNameDialog(const juce::String& defaultDeviceName) : Dialog(BinaryData::upload_svg, "Change Device Name")
{
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(nameValue);

    nameValue.setText(defaultDeviceName, false);

    setSize(dialogWidth, calculateHeight(1));
}

void ChangeDeviceNameDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds().removeFromTop(UILayout::textComponentHeight);

    nameLabel.setBounds(bounds.removeFromLeft(columnWidth));
    nameValue.setBounds(bounds);
}

juce::String ChangeDeviceNameDialog::getDeviceName() const
{
    return nameValue.getText();
}
