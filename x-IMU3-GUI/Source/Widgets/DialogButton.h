#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class DialogButton : public juce::TextButton
{
public:
    explicit DialogButton(const juce::String& buttonName);

    ~DialogButton() override;

private:
    class LookAndFeel;

    std::unique_ptr<LookAndFeel> lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DialogButton)
};
