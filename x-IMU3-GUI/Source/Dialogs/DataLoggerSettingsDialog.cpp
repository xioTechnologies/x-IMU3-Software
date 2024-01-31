#include "DataLoggerSettingsDialog.h"
#include <filesystem>

DataLoggerSettingsDialog::DataLoggerSettingsDialog(const Settings& settings) : Dialog(BinaryData::settings_svg, "Data Logger Settings", "Start")
{
    addAndMakeVisible(directoryLabel);
    addAndMakeVisible(directoryValue);
    addAndMakeVisible(directoryButton);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(nameValue);
    addAndMakeVisible(timeLabel);
    addAndMakeVisible(timeValue);
    addAndMakeVisible(timeUnit);

    juce::File(settings.directory).createDirectory();

    directoryButton.onClick = [&]
    {
        juce::FileChooser fileChooser(directoryButton.getTooltip(), std::filesystem::exists(directoryValue.getText().toStdString()) ? directoryValue.getText() : "");
        if (fileChooser.browseForDirectory())
        {
            directoryValue.setText(fileChooser.getResult().getFullPathName(), true);
        }
    };

    directoryValue.setText(settings.directory, false);
    nameValue.setTextToShowWhenEmpty("Logged Data " + juce::Time::getCurrentTime().formatted("%Y-%m-%d %H-%M-%S"), juce::Colours::grey);
    nameValue.setText(settings.nameEmpty ? "" : settings.name, false);
    timeValue.setText(juce::String(settings.timeValue), false);
    timeUnit.addItemList({ "Unlimited", "Hours", "Minutes", "Seconds" }, 1);
    timeUnit.setSelectedItemIndex(static_cast<int>(settings.timeUnit), juce::dontSendNotification);

    directoryValue.onTextChange = nameValue.onTextChange = [&]
    {
        setOkButton(std::filesystem::exists(directoryValue.getText().toStdString()));
    };
    directoryValue.onTextChange();

    timeUnit.onChange = [&]
    {
        timeValue.setEnabled(timeUnit.getSelectedItemIndex() > 0);
        timeValue.applyColourToAllText(timeUnit.getSelectedItemIndex() > 0 ? findColour(juce::TextEditor::textColourId) : juce::Colour());
    };
    timeUnit.onChange();

    setSize(600, calculateHeight(3));
}

void DataLoggerSettingsDialog::resized()
{
    Dialog::resized();
    auto bounds = getContentBounds();

    auto directoryRow = bounds.removeFromTop(UILayout::textComponentHeight);
    directoryLabel.setBounds(directoryRow.removeFromLeft(columnWidth));
    directoryButton.setBounds(directoryRow.removeFromRight(iconButtonWidth));
    directoryValue.setBounds(directoryRow.withTrimmedRight(margin));

    bounds.removeFromTop(Dialog::margin);

    auto nameRow = bounds.removeFromTop(UILayout::textComponentHeight);
    nameLabel.setBounds(nameRow.removeFromLeft(columnWidth));
    nameValue.setBounds(nameRow.removeFromLeft(2 * columnWidth + margin));

    bounds.removeFromTop(Dialog::margin);

    auto secondsRow = bounds.removeFromTop(UILayout::textComponentHeight);
    timeLabel.setBounds(secondsRow.removeFromLeft(columnWidth));
    timeValue.setBounds(secondsRow.removeFromLeft(columnWidth));
    secondsRow.removeFromLeft(margin);
    timeUnit.setBounds(secondsRow.removeFromLeft(columnWidth));
}

DataLoggerSettingsDialog::Settings DataLoggerSettingsDialog::getSettings() const
{
    Settings settings;
    settings.directory = directoryValue.getText();
    settings.name = nameValue.getText().isNotEmpty() ? nameValue.getText() : nameValue.getTextToShowWhenEmpty();
    settings.nameEmpty = nameValue.getText().isEmpty();
    settings.timeValue = timeValue.getText().getFloatValue();
    settings.timeUnit = static_cast<Settings::TimeUnit>(timeUnit.getSelectedItemIndex());
    return settings;
}
