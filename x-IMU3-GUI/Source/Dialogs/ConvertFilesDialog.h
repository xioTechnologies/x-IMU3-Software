#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/FileSelector.h"
#include "Widgets/SimpleLabel.h"

class ConvertFilesDialog : public Dialog
{
public:
    struct Settings
    {
        juce::Array<juce::File> files;
        juce::File destination;
        bool destinationEmpty;
        juce::String name;
        bool nameEmpty;
    };

    ConvertFilesDialog(const Settings& settings);

    void resized() override;

    Settings getSettings() const;

private:
    SimpleLabel filesLabel { ".ximu3 Files:" };
    FileSelector filesSelector { "Select .ximu3 Files", ".ximu3" };
    SimpleLabel destinationLabel { "Destination:" };
    FileSelector destinationSelector { "Select Destination Directory", {} };
    SimpleLabel nameLabel { "Name:" };
    CustomTextEditor nameValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvertFilesDialog)
};
