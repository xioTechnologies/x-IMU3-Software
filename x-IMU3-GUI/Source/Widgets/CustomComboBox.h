#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CustomComboBox : public juce::ComboBox
{
public:
    CustomComboBox();

    ~CustomComboBox() override;

    bool keyPressed(const juce::KeyPress& key) override;

private:
    class LookAndFeel;

    std::unique_ptr<LookAndFeel> lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomComboBox)
};
