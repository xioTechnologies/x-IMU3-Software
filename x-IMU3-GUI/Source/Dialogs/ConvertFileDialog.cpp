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
        if (fileChooser.browseForMultipleFilesToOpen())
        {
            const auto files = fileChooser.getResults();

            juce::String text;
            for (auto file : files)
            {
                text += file.getFullPathName() + ";";
            }
            text = text.dropLastCharacters(1);
            sourceValue.setText(text);

            if (destinationValue.isEmpty())
            {
                destinationValue.setText(files[0].getParentDirectory().getFullPathName());
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
        setOkButton(std::filesystem::exists(destinationValue.getText().toStdString()));

        for (const auto& source : getSources())
        {
            if (std::filesystem::exists(source.toStdString()) == false)
            {
                setOkButton(false);
            }
        }
    };
    setOkButton(false);

    setSize(600, calculateHeight(2));
}

void ConvertFileDialog::resized()
{
    Dialog::resized();
    auto bounds = getContentBounds();

    auto sourceRow = bounds.removeFromTop(UILayout::textComponentHeight);
    sourceLabel.setBounds(sourceRow.removeFromLeft(columnWidth));
    sourceButton.setBounds(sourceRow.removeFromRight(iconButtonWidth));
    sourceValue.setBounds(sourceRow.withTrimmedRight(margin));

    bounds.removeFromTop(Dialog::margin);

    auto destinationRow = bounds.removeFromTop(UILayout::textComponentHeight);
    destinationLabel.setBounds(destinationRow.removeFromLeft(columnWidth));
    destinationButton.setBounds(destinationRow.removeFromRight(iconButtonWidth));
    destinationValue.setBounds(destinationRow.withTrimmedRight(margin));
}

juce::StringArray ConvertFileDialog::getSources() const
{
    return juce::StringArray::fromTokens(sourceValue.getText(), ";", "");
}

juce::String ConvertFileDialog::getDestination() const
{
    return destinationValue.getText();
}
