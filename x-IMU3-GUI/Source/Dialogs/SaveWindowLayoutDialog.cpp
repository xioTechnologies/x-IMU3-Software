#include <BinaryData.h>
#include "SaveWindowLayoutDialog.h"

SaveWindowLayoutDialog::SaveWindowLayoutDialog() : Dialog(BinaryData::save_svg, "Save Window Layout", "Save")
{
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(nameValue);

    nameValue.onTextChange = [this]
    {
        setOkButton(getLayoutName().isNotEmpty() && juce::File::createLegalFileName(getLayoutName()) == getLayoutName());
    };

    nameValue.setText("Window Layout", false);

    setSize(dialogWidth, calculateHeight(1));
}

void SaveWindowLayoutDialog::resized()
{
    Dialog::resized();

    auto bounds = getContentBounds().removeFromTop(UILayout::textComponentHeight);

    nameLabel.setBounds(bounds.removeFromLeft(columnWidth));
    nameValue.setBounds(bounds);
}

juce::String SaveWindowLayoutDialog::getLayoutName() const
{
    return nameValue.getText().trim();
}
