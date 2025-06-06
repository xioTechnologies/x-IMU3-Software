#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Ximu3.hpp"

class UpdatingFirmwareDialog : public Dialog
{
public:
    UpdatingFirmwareDialog(std::shared_ptr<ximu3::ConnectionInfo> connectionInfo_, const juce::File& hexFile_, juce::ThreadPool& threadPool);

    void resized() override;

private:
    const std::shared_ptr<ximu3::ConnectionInfo> connectionInfo;
    const juce::File hexFile;

    double progressBarValue = 0.0;
    juce::ProgressBar progressBar { progressBarValue };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpdatingFirmwareDialog)
};
