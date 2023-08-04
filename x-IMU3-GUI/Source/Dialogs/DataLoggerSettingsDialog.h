#pragma once

#include "../ApplicationSettings.h"
#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomComboBox.h"
#include "Widgets/CustomTextEditor.h"
#include "Widgets/CustomToggleButton.h"
#include "Widgets/Icon.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class DataLoggerSettingsDialog : public Dialog
{
public:
    struct Settings
    {
        juce::String directory = ApplicationSettings::getDirectory().getChildFile("Data Logger").getFullPathName();
        juce::String name = "Logged Data";
        bool appendDateAndTime = true;
        float timeValue = 10.0f;
        enum class TimeUnit
        {
            unlimited,
            hours,
            minutes,
            seconds,
        };
        TimeUnit timeUnit = TimeUnit::unlimited;

        std::optional<juce::RelativeTime> getTime() const
        {
            switch (timeUnit)
            {
                case TimeUnit::unlimited:
                    return {};
                case TimeUnit::hours:
                    return juce::RelativeTime::hours((double) timeValue);
                case TimeUnit::minutes:
                    return juce::RelativeTime::minutes((double) timeValue);
                case TimeUnit::seconds:
                    return juce::RelativeTime::seconds((double) timeValue);
            }
            return {};
        }
    };

    explicit DataLoggerSettingsDialog(const Settings& settings);

    void resized() override;

    Settings getSettings() const;

private:
    SimpleLabel directoryLabel { "Directory:" };
    CustomTextEditor directoryValue;
    IconButton directoryButton { BinaryData::open_svg, "Select Directory" };
    SimpleLabel nameLabel { "Name:" };
    CustomTextEditor nameValue;
    CustomToggleButton appendDateAndTimeToggle { "Append Date and Time" };
    SimpleLabel timeLabel { "Time:" };
    CustomTextEditor timeValue;
    CustomComboBox timeUnit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataLoggerSettingsDialog)
};
