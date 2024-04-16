#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomTextEditor.h"
#include "Widgets/FileSelector.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class ConvertFilesDialog : public Dialog
{
public:
    ConvertFilesDialog();

    void resized() override;

    juce::Array<juce::File> getFiles() const;

    juce::File getDestination() const;

private:
    SimpleLabel filesLabel { ".ximu3 Files:" };
    FileSelector filesSelector { "Select .ximu3 Files", ".ximu3" };
    SimpleLabel destinationLabel { "Destination:" };
    FileSelector destinationSelector { "Select Destination Directory", {} };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvertFilesDialog)
};
