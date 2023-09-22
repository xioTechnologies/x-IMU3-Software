#pragma once

#include "Dialog.h"
#include "Widgets/IconButton.h"

class ApplicationSettingsDialog : public Dialog
{
public:
    ApplicationSettingsDialog();

    void resized() override;

private:
    std::vector<std::unique_ptr<juce::Component>> groups;

    IconButton defaultsButton { BinaryData::default_svg, "Restore Defaults" };

    void initialiseGroups();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApplicationSettingsDialog)
};
