#include "ConvertFilesDialog.h"
#include <filesystem>

ConvertFilesDialog::ConvertFilesDialog() : Dialog(BinaryData::tools_svg, "Convert .ximu3 Files", "Convert")
{
    addAndMakeVisible(filesLabel);
    addAndMakeVisible(filesValue);
    addAndMakeVisible(filesButton);
    addAndMakeVisible(destinationLabel);
    addAndMakeVisible(destinationValue);
    addAndMakeVisible(destinationButton);

    filesButton.onClick = [&]
    {
        juce::FileChooser fileChooser(filesButton.getTooltip(), std::filesystem::exists(filesValue.getText().toStdString()) ? filesValue.getText() : "", "*.ximu3");
        if (fileChooser.browseForMultipleFilesToOpen())
        {
            const auto files = fileChooser.getResults();

            juce::String text;
            for (auto file : files)
            {
                text += file.getFullPathName() + ";";
            }
            text = text.dropLastCharacters(1);
            filesValue.setText(text);

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

    filesValue.onTextChange = destinationValue.onTextChange = [&]
    {
        auto valid = std::filesystem::exists(destinationValue.getText().toStdString());
        for (const auto& file : getFilesAsStrings())
        {
            valid = valid && std::filesystem::exists(file.toStdString());
        }
        setOkButton(valid);
    };
    setOkButton(false);

    setSize(600, calculateHeight(2));
}

void ConvertFilesDialog::resized()
{
    Dialog::resized();
    auto bounds = getContentBounds();

    auto filesRow = bounds.removeFromTop(UILayout::textComponentHeight);
    filesLabel.setBounds(filesRow.removeFromLeft(columnWidth));
    filesButton.setBounds(filesRow.removeFromRight(iconButtonWidth));
    filesValue.setBounds(filesRow.withTrimmedRight(margin));

    bounds.removeFromTop(Dialog::margin);

    auto destinationRow = bounds.removeFromTop(UILayout::textComponentHeight);
    destinationLabel.setBounds(destinationRow.removeFromLeft(columnWidth));
    destinationButton.setBounds(destinationRow.removeFromRight(iconButtonWidth));
    destinationValue.setBounds(destinationRow.withTrimmedRight(margin));
}

std::vector<juce::File> ConvertFilesDialog::getFiles() const
{
    std::vector<juce::File> files;
    for (auto file : getFilesAsStrings())
    {
        files.push_back(file);
    }
    return files;
}

juce::File ConvertFilesDialog::getDestination() const
{
    return destinationValue.getText();
}

juce::StringArray ConvertFilesDialog::getFilesAsStrings() const
{
    return juce::StringArray::fromTokens(filesValue.getText(), ";", "");
}
