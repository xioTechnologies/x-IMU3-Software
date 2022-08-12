#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Ximu3.hpp"

class UpdatingFirmwareDialog : public Dialog, private juce::Thread
{
public:
    UpdatingFirmwareDialog(std::unique_ptr<ximu3::ConnectionInfo> connectionInfo_, const juce::String& fileName_);

    ~UpdatingFirmwareDialog() override;

    void resized() override;

private:
    const std::unique_ptr<ximu3::ConnectionInfo> connectionInfo;
    const juce::String fileName;

    double progressBarValue = 0.0;
    juce::ProgressBar progressBar { progressBarValue };

    void run() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UpdatingFirmwareDialog)
};
