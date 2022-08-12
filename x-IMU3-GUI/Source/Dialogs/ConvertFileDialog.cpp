#include "ConvertFileDialog.h"
#include <filesystem>

ConvertFileDialog::ConvertFileDialog() : Dialog(BinaryData::tools_svg, "Convert File (.ximu3)", "Convert")
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

    setSize(600, calculateHeight(2));
}

void ConvertFileDialog::resized()
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

juce::String ConvertFileDialog::getSource() const
{
    return sourceValue.getText();
}

juce::String ConvertFileDialog::getDestination() const
{
    return destinationValue.getText();
}
