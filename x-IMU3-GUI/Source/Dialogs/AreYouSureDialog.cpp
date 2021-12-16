#include "AreYouSureDialog.h"

AreYouSureDialog::AreYouSureDialog(const juce::String& text) : Dialog(BinaryData::warning_white_svg, "Are You Sure?", "Yes", "No"), label(text)
{
    addAndMakeVisible(label);

    setSize(dialogWidth, calculateHeight(1));
}

void AreYouSureDialog::resized()
{
    Dialog::resized();

    label.setBounds(getContentBounds());
}
