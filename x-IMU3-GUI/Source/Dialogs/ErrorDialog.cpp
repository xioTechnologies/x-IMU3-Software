#include "ErrorDialog.h"

ErrorDialog::ErrorDialog(const juce::String& error) : Dialog(BinaryData::warning_white_svg, "Error", "OK", "")
{
    addAndMakeVisible(label);

    label.setText(error);

    setSize(dialogWidth, calculateHeight(1));
}

void ErrorDialog::resized()
{
    Dialog::resized();

    label.setBounds(getContentBounds().removeFromTop(UILayout::textComponentHeight));
}
