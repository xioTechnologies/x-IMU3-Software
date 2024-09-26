#include "DataLoggerSettingsDialog.h"
#include "CustomLookAndFeel.h"

DataLoggerSettingsDialog::DataLoggerSettingsDialog(const Settings& settings) : Dialog(BinaryData::settings_svg, "Data Logger Settings", "Start")
{
    addAndMakeVisible(destinationLabel);
    addAndMakeVisible(destinationSelector);
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(nameValue);
    addAndMakeVisible(timeLabel);
    addAndMakeVisible(timeValue);
    addAndMakeVisible(timeUnit);

    settings.destination.createDirectory();

    destinationSelector.setFiles({settings.destination});
    nameValue.setDefaultText("Logged Data " + juce::Time::getCurrentTime().formatted("%Y-%m-%d %H-%M-%S"));
    nameValue.setText(settings.nameEmpty ? "" : settings.name, false);
    timeValue.setText(juce::String(settings.timeValue), false);
    timeUnit.addItemList({ "Unlimited", "Hours", "Minutes", "Seconds" }, 1);
    timeUnit.setSelectedItemIndex(static_cast<int>(settings.timeUnit), juce::dontSendNotification);

    destinationSelector.onChange = nameValue.onTextChange = [&]
    {
        setOkButton(destinationSelector.isValid());
    };
    destinationSelector.onChange();

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

    auto destinationRow = bounds.removeFromTop(UILayout::textComponentHeight);
    destinationLabel.setBounds(destinationRow.removeFromLeft(columnWidth));
    destinationSelector.setBounds(destinationRow);

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
    settings.destination = destinationSelector.getFiles()[0];
    settings.name = nameValue.getTextOrDefault().trim();
    settings.nameEmpty = nameValue.getText().isEmpty();
    settings.timeValue = timeValue.getText().getFloatValue();
    settings.timeUnit = static_cast<Settings::TimeUnit>(timeUnit.getSelectedItemIndex());
    return settings;
}
