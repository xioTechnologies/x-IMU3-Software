#include "AreYouSureDialog.h"
#include <BinaryData.h>

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

DoYouWantToReplaceItDialog::DoYouWantToReplaceItDialog(const juce::String& name) : AreYouSureDialog(name.quoted() + " already exists. Do you want to replace it?")
{
}
