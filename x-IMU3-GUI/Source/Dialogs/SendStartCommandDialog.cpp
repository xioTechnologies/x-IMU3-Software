#include "SendStartCommandDialog.h"
#include <BinaryData.h>

SendStartCommandDialog::SendStartCommandDialog(const juce::String& title) : Dialog(BinaryData::microSD_svg, title, "Send", "Cancel")
{
    addAndMakeVisible(label);
    addAndMakeVisible(value);

    setSize(600, calculateHeight(1));
}

void SendStartCommandDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds();
    label.setBounds(bounds.removeFromLeft(columnWidth));
    value.setBounds(bounds);
}

juce::String SendStartCommandDialog::getFileName() const
{
    return value.getText();
}
