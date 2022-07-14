#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/SimpleLabel.h"
#include "Ximu3.hpp"

class ConvertingFileDialog : public Dialog, private juce::Timer
{
public:
    ConvertingFileDialog(const juce::String& source, const juce::String& destination);

    void resized() override;

private:
    const juce::File file;

    double progressBarValue = 0.0;
    juce::ProgressBar progressBar { progressBarValue };

    ximu3::FileConverter fileConverter;

    void progressCallback(ximu3::XIMU3_FileConverterProgress);

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvertingFileDialog)
};
