#pragma once

#include "../CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class CustomTextEditor : public juce::TextEditor
{
public:
    CustomTextEditor()
    {
        setColour(juce::TextEditor::backgroundColourId, UIColours::textEditor);
        setColour(juce::TextEditor::textColourId, UIColours::background);
        setFont(UIFonts::getDefaultFont());
        setSelectAllWhenFocused(true);
        setEscapeAndReturnKeysConsumed(false);
    }

    void focusLost(FocusChangeType cause) override
    {
        juce::TextEditor::focusLost(cause);
        setHighlightedRegion({});
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomTextEditor)
};
