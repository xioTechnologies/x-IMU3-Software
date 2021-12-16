#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CustomToggleButton : public juce::ToggleButton
{
public:
    explicit CustomToggleButton(const juce::String& buttonName);

    ~CustomToggleButton() override;

private:
    class LookAndFeel;

    std::unique_ptr<LookAndFeel> lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomToggleButton)
};
