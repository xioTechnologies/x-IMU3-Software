#include "FileConverterDialog.h"

FileConverterDialog::FileConverterDialog() : Dialog(BinaryData::tools_svg, "File Converter", "Convert")
{
    addAndMakeVisible(sourceLabel);
    addAndMakeVisible(sourceValue);
    addAndMakeVisible(sourceButton);
    addAndMakeVisible(destinationLabel);
    addAndMakeVisible(destinationValue);
    addAndMakeVisible(destinationButton);

    sourceButton.onClick = [&]
    {
        juce::FileChooser fileChooser(sourceButton.getTooltip(), juce::File(sourceValue.getText()).getParentDirectory(), "*.ximu3");
        if (fileChooser.browseForFileToOpen())
        {
            sourceValue.setText(fileChooser.getResult().getFullPathName(), false);
            if (destinationValue.isEmpty())
            {
                destinationValue.setText(juce::File::addTrailingSeparator(fileChooser.getResult().getParentDirectory().getFullPathName()), false);
            }
        }
    };

    destinationButton.onClick = [&]
    {
        juce::FileChooser fileChooser(destinationButton.getTooltip(), juce::File(destinationValue.getText()));
        if (fileChooser.browseForDirectory())
        {
            destinationValue.setText(juce::File::addTrailingSeparator(fileChooser.getResult().getFullPathName()), false);
        }
    };

    setSize(dialogWidth, calculateHeight(2));
}

void FileConverterDialog::resized()
{
    Dialog::resized();
    auto bounds = getContentBounds();

    auto sourceRow = bounds.removeFromTop(UILayout::textComponentHeight);
    sourceLabel.setBounds(sourceRow.removeFromLeft(columnWidth));
    sourceButton.setBounds(sourceRow.removeFromRight(50));
    sourceValue.setBounds(sourceRow.withTrimmedRight(margin));

    bounds.removeFromTop(Dialog::margin);

    auto destinationRow = bounds.removeFromTop(UILayout::textComponentHeight);
    destinationLabel.setBounds(destinationRow.removeFromLeft(columnWidth));
    destinationButton.setBounds(destinationRow.removeFromRight(50));
    destinationValue.setBounds(destinationRow.withTrimmedRight(margin));
}

juce::String FileConverterDialog::getSource() const
{
    return sourceValue.getText();
}

juce::String FileConverterDialog::getDestination() const
{
    return destinationValue.getText();
}
