#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class ConvertingFileDialog : public Dialog, private juce::Timer
{
public:
    static void show(const juce::StringArray& sources_, const juce::String& destination_);

    ConvertingFileDialog(const juce::StringArray& sources_, const juce::String& destination_);

    void resized() override;

private:
    juce::StringArray sources;
    const juce::String destination;

    const juce::File file;

    double progressBarValue = 0.0;
    juce::ProgressBar progressBar { progressBarValue };

    ximu3::FileConverter fileConverter;

    void progressCallback(ximu3::XIMU3_FileConverterProgress);

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvertingFileDialog)
};
