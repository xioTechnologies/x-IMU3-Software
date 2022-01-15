#include "FileConverterDialog.h"
#include <filesystem>

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
        juce::FileChooser fileChooser(sourceButton.getTooltip(), std::filesystem::exists(sourceValue.getText().toStdString()) ? sourceValue.getText() : "", "*.ximu3");
        if (fileChooser.browseForFileToOpen())
        {
            sourceValue.setText(fileChooser.getResult().getFullPathName());
            if (destinationValue.isEmpty())
            {
                destinationValue.setText(fileChooser.getResult().getParentDirectory().getFullPathName());
            }
        }
    };

    destinationButton.onClick = [&]
    {
        juce::FileChooser fileChooser(destinationButton.getTooltip(), std::filesystem::exists(destinationValue.getText().toStdString()) ? destinationValue.getText() : "");
        if (fileChooser.browseForDirectory())
        {
            destinationValue.setText(fileChooser.getResult().getFullPathName());
        }
    };

    sourceValue.onTextChange = destinationValue.onTextChange = [&]
    {
        setValid(std::filesystem::exists(sourceValue.getText().toStdString()) && std::filesystem::exists(destinationValue.getText().toStdString()));
    };
    setValid(false);

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
