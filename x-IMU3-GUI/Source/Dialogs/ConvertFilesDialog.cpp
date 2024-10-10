#include "ConvertFilesDialog.h"

ConvertFilesDialog::ConvertFilesDialog(const Settings& settings) : Dialog(BinaryData::tools_svg, "Convert .ximu3 Files", "Convert")
{
    addAndMakeVisible(filesLabel);
    addAndMakeVisible(filesSelector);
    addAndMakeVisible(destinationLabel);
    addAndMakeVisible(destinationSelector);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(nameValue);

    filesSelector.setFiles(settings.files);
    if (settings.destinationEmpty == false)
    {
        destinationSelector.setFiles({ settings.destination });
    }
    if (settings.nameEmpty == false)
    {
        nameValue.setText(settings.name);
    }

    filesSelector.onChange = destinationSelector.onChange = nameValue.onTextChange = [&]
    {
        const auto files = filesSelector.getFiles();
        destinationSelector.setFilesWhenEmpty({ files[0].getParentDirectory() });
        nameValue.setDefaultText((files.size() == 1) ? files[0].getFileNameWithoutExtension() : "");

        setOkButton(filesSelector.isValid() && destinationSelector.isValid() && nameValue.getTextOrDefault().isNotEmpty());
    };

    filesSelector.onChange();

    setSize(600, calculateHeight(3));
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

    bounds.removeFromTop(Dialog::margin);

    auto nameRow = bounds.removeFromTop(UILayout::textComponentHeight);
    nameLabel.setBounds(nameRow.removeFromLeft(columnWidth));
    nameValue.setBounds(nameRow.removeFromLeft(2 * columnWidth));
}

ConvertFilesDialog::Settings ConvertFilesDialog::getSettings() const
{
    Settings settings;
    settings.files = filesSelector.getFiles();
    settings.destination = destinationSelector.getFiles()[0];
    settings.destinationEmpty = destinationSelector.isEmpty();
    settings.name = nameValue.getTextOrDefault();
    settings.nameEmpty = nameValue.isEmpty();
    return settings;
}
