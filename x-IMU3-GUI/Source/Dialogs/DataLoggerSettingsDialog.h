#pragma once

#include "../ApplicationSettings.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomTextEditor.h"
#include "Widgets/CustomToggleButton.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class DataLoggerSettingsDialog : public Dialog
{
public:
    struct Settings
    {
        juce::String directory = juce::File::addTrailingSeparator(ApplicationSettings::getDirectory().getChildFile("Data Logger").getFullPathName());
        juce::String name = "Logged Data";
        int seconds = 60;
        bool unlimited = true;
    };

    explicit DataLoggerSettingsDialog(const Settings& settings);

    void resized() override;

    Settings getSettings() const;

private:
    SimpleLabel directoryLabel { "Directory:" };
    CustomTextEditor directoryValue;
    IconButton directoryButton { IconButton::Style::menuStrip, BinaryData::open_svg, 0.8f, "Select Directory" };
    SimpleLabel nameLabel { "Name:" };
    CustomTextEditor nameValue;
    SimpleLabel secondsLabel { "Seconds:" };
    CustomTextEditor secondsValue;
    CustomToggleButton unlimitedToggle { "Unlimited" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataLoggerSettingsDialog)
};
