#pragma once

#include "CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class CustomTextEditor : public juce::TextEditor
{
public:
    CustomTextEditor()
    {
        setFont(UIFonts::getDefaultFont());
        setSelectAllWhenFocused(true);
        setEscapeAndReturnKeysConsumed(false);
    }

    void focusLost(FocusChangeType cause) override
    {
        juce::TextEditor::focusLost(cause);
        setHighlightedRegion({});
    }

    void setDefaultText(const juce::String& defaultText)
    {
        setTextToShowWhenEmpty(defaultText, juce::Colours::grey);
        repaint();
    }

    juce::String getTextOrDefault() const
    {
        return getText().isEmpty() ? getTextToShowWhenEmpty() : getText();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomTextEditor)
};
