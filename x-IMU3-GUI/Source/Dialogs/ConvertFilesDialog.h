#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class ConvertFilesDialog : public Dialog
{
public:
    ConvertFilesDialog();

    void resized() override;

    std::vector<juce::File> getFiles() const;

    juce::File getDestination() const;

private:
    SimpleLabel filesLabel { ".ximu3 Files:" };
    CustomTextEditor filesValue;
    IconButton filesButton { BinaryData::open_svg, "Select .ximu3 Files" };
    SimpleLabel destinationLabel { "Destination:" };
    CustomTextEditor destinationValue;
    IconButton destinationButton { BinaryData::open_svg, "Select Destination Directory" };

    juce::StringArray getFilesAsStrings() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvertFilesDialog)
};
