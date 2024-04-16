#include "DataLoggerSettingsDialog.h"

DataLoggerSettingsDialog::DataLoggerSettingsDialog(const Settings& settings) : Dialog(BinaryData::settings_svg, "Data Logger Settings", "Start")
{
    addAndMakeVisible(directoryLabel);
    addAndMakeVisible(directorySelector);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(nameValue);
    addAndMakeVisible(timeLabel);
    addAndMakeVisible(timeValue);
    addAndMakeVisible(timeUnit);

    juce::File(settings.directory).createDirectory();

    directorySelector.setFiles({settings.directory});
    nameValue.setTextToShowWhenEmpty("Logged Data " + juce::Time::getCurrentTime().formatted("%Y-%m-%d %H-%M-%S"), juce::Colours::grey);
    nameValue.setText(settings.nameEmpty ? "" : settings.name, false);
    timeValue.setText(juce::String(settings.timeValue), false);
    timeUnit.addItemList({ "Unlimited", "Hours", "Minutes", "Seconds" }, 1);
    timeUnit.setSelectedItemIndex(static_cast<int>(settings.timeUnit), juce::dontSendNotification);

    directorySelector.onChange = nameValue.onTextChange = [&]
    {
        setOkButton(directorySelector.isValid());
    };
    directorySelector.onChange();

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
    directorySelector.setBounds(directoryRow);

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
    settings.directory = directorySelector.getFiles()[0].getFullPathName();
    settings.name = nameValue.getText().isNotEmpty() ? nameValue.getText().trim() : nameValue.getTextToShowWhenEmpty();
    settings.nameEmpty = nameValue.getText().isEmpty();
    settings.timeValue = timeValue.getText().getFloatValue();
    settings.timeUnit = static_cast<Settings::TimeUnit>(timeUnit.getSelectedItemIndex());
    return settings;
}
