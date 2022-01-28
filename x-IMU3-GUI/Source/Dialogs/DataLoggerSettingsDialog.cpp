#include "DataLoggerSettingsDialog.h"
#include <filesystem>

DataLoggerSettingsDialog::DataLoggerSettingsDialog(const Settings& settings) : Dialog(BinaryData::settings_svg, "Data Logger Settings", "Start")
{
    addAndMakeVisible(directoryLabel);
    addAndMakeVisible(directoryValue);
    addAndMakeVisible(directoryButton);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(nameValue);
    addChildComponent(warningIcon);
    addAndMakeVisible(secondsLabel);
    addAndMakeVisible(secondsValue);
    addAndMakeVisible(unlimitedToggle);

    directoryButton.onClick = [&]
    {
        juce::FileChooser fileChooser(directoryButton.getTooltip(), std::filesystem::exists(directoryValue.getText().toStdString()) ? directoryValue.getText() : "");
        if (fileChooser.browseForDirectory())
        {
            directoryValue.setText(fileChooser.getResult().getFullPathName(), true);
        }
    };

    unlimitedToggle.onClick = [this]
    {
        secondsValue.setEnabled(!unlimitedToggle.getToggleState());
        secondsValue.applyFontToAllText(secondsValue.isEnabled() ? UIFonts::defaultFont : UIFonts::defaultFont.withHorizontalScale(0.0f));
    };

    directoryValue.setText(settings.directory, false);
    nameValue.setText(settings.name, false);
    secondsValue.setText(juce::String(settings.seconds), false);
    unlimitedToggle.setToggleState(settings.unlimited, juce::sendNotificationSync);

    directoryValue.onTextChange = nameValue.onTextChange = [&]
    {
        const auto directoryExists = std::filesystem::exists(directoryValue.getText().toStdString());
        const auto fileValid = juce::File(directoryValue.getText()).getChildFile(nameValue.getText()).exists() == false && juce::File::createLegalFileName(nameValue.getText()) == nameValue.getText();
        setValid(directoryExists && fileValid);
        warningIcon.setVisible(directoryExists && fileValid == false);
    };
    directoryValue.onTextChange();

    setSize(dialogWidth, calculateHeight(3));
}

void DataLoggerSettingsDialog::resized()
{
    Dialog::resized();
    auto bounds = getContentBounds();

    auto directoryRow = bounds.removeFromTop(UILayout::textComponentHeight);
    directoryLabel.setBounds(directoryRow.removeFromLeft(columnWidth));
    directoryButton.setBounds(directoryRow.removeFromRight(50));
    directoryValue.setBounds(directoryRow.withTrimmedRight(margin));

    bounds.removeFromTop(Dialog::margin);

    auto nameRow = bounds.removeFromTop(UILayout::textComponentHeight);
    nameLabel.setBounds(nameRow.removeFromLeft(columnWidth));
    nameValue.setBounds(nameRow.removeFromLeft(2 * columnWidth));
    nameRow.removeFromLeft(margin);
    warningIcon.setBounds(nameRow.removeFromLeft(20));

    bounds.removeFromTop(Dialog::margin);

    auto secondsRow = bounds.removeFromTop(UILayout::textComponentHeight);
    secondsLabel.setBounds(secondsRow.removeFromLeft(columnWidth));
    secondsValue.setBounds(secondsRow.removeFromLeft(columnWidth));
    secondsRow.removeFromLeft(margin);
    unlimitedToggle.setBounds(secondsRow);
}

DataLoggerSettingsDialog::Settings DataLoggerSettingsDialog::getSettings() const
{
    Settings settings;
    settings.directory = directoryValue.getText();
    settings.name = nameValue.getText();
    settings.seconds = secondsValue.getText().getIntValue();
    settings.unlimited = unlimitedToggle.getToggleState();
    return settings;
}
