#include "ConvertFilesDialog.h"

ConvertFilesDialog::ConvertFilesDialog() : Dialog(BinaryData::tools_svg, "Convert .ximu3 Files", "Convert")
{
    addAndMakeVisible(filesLabel);
    addAndMakeVisible(filesSelector);
    addAndMakeVisible(destinationLabel);
    addAndMakeVisible(destinationSelector);

    destinationSelector.onChange = [&]
    {
        setOkButton(filesSelector.isValid() && destinationSelector.isValid());
    };
    filesSelector.onChange = [&]
    {
        setOkButton(filesSelector.isValid() && destinationSelector.isValid());

        if (filesSelector.isValid() && destinationSelector.getFiles().isEmpty())
        {
            destinationSelector.setFiles({filesSelector.getFiles()[0].getParentDirectory().getFullPathName()});
        }
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
    filesSelector.setBounds(filesRow);

    bounds.removeFromTop(Dialog::margin);

    auto destinationRow = bounds.removeFromTop(UILayout::textComponentHeight);
    destinationLabel.setBounds(destinationRow.removeFromLeft(columnWidth));
    destinationSelector.setBounds(destinationRow);
}

juce::Array<juce::File> ConvertFilesDialog::getFiles() const
{
    return filesSelector.getFiles();
}

juce::File ConvertFilesDialog::getDestination() const
{
    return destinationSelector.getFiles()[0];
}
