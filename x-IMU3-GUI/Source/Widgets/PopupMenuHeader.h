#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class PopupMenuHeader : public juce::PopupMenu::CustomComponent
{
public:
    explicit PopupMenuHeader(const juce::String& title_)
            : juce::PopupMenu::CustomComponent(false),
              title(title_)
    {
    }

    void paint(juce::Graphics& g) override
    {
        getLookAndFeel().drawPopupMenuSectionHeader(g, getLocalBounds(), title);
    }

    void getIdealSize(int& idealWidth, int& idealHeight) override
    {
        idealWidth = 50;
        idealHeight = 15;
    }

private:
    const juce::String title;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PopupMenuHeader)
};
