#pragma once

#include "Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "Widgets/CustomTextEditor.h"
#include "Widgets/IconButton.h"
#include "Widgets/SimpleLabel.h"

class ConvertFileDialog : public Dialog
{
public:
    ConvertFileDialog();

    void resized() override;

    juce::String getSource() const;

    juce::String getDestination() const;

private:
    SimpleLabel sourceLabel { "Source:" };
    CustomTextEditor sourceValue;
    IconButton sourceButton { BinaryData::open_svg, "Select Source File" };
    SimpleLabel destinationLabel { "Destination:" };
    CustomTextEditor destinationValue;
    IconButton destinationButton { BinaryData::open_svg, "Select Destination Directory" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvertFileDialog)
};
