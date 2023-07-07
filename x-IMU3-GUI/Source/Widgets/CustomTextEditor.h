#pragma once

#include "../CustomLookAndFeel.h"
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

    void paintOverChildren(juce::Graphics& g) override
    {
        if (getTextToShowWhenEmpty().isNotEmpty()
            && (!hasKeyboardFocus(false))
            && getTotalNumChars() == 0)
        {
            g.setColour(juce::Colours::grey);
            g.setFont(getFont().italicised());

            juce::Rectangle<int> textBounds(getLeftIndent(),
                                            getTopIndent(),
                                            getWidth() - getBorder().getLeftAndRight() - getLeftIndent(),
                                            getHeight() - getTopIndent());

            if (!textBounds.isEmpty())
            {
                g.drawText(getTextToShowWhenEmpty(), textBounds, getJustificationType(), true);
            }
        }

        getLookAndFeel().drawTextEditorOutline(g, getWidth(), getHeight(), *this);
    }

    void focusLost(FocusChangeType cause) override
    {
        juce::TextEditor::focusLost(cause);
        setHighlightedRegion({});
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomTextEditor)
};
