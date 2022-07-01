#pragma once

#include "../Widgets/IconButton.h"
#include "Dialog.h"

class ApplicationSettingsDialog : public Dialog
{
public:
    ApplicationSettingsDialog();

    void resized() override;

private:
    IconButton defaultButton { IconButton::Style::menuStrip, BinaryData::default_svg, 0.8f, "Restore Default Application Settings" };

    std::vector<std::unique_ptr<juce::Component>> groups;

    void initialiseGroups();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApplicationSettingsDialog)
};
