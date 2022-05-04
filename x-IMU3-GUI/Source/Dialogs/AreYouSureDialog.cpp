#include "AreYouSureDialog.h"

AreYouSureDialog::AreYouSureDialog(const juce::String& text) : Dialog(BinaryData::warning_white_svg, "Are You Sure?", "Yes", "No"), label(text)
{
    addAndMakeVisible(label);

    setSize(juce::jmax(dialogWidth, 2 * margin + (int) std::ceil(label.getTextWidth())), calculateHeight(1));
}

void AreYouSureDialog::resized()
{
    Dialog::resized();

    label.setBounds(getContentBounds());
}
