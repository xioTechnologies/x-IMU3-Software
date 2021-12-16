#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class Stopwatch : public juce::TextButton
{
public:
    Stopwatch();

    ~Stopwatch() override;

    void setTime(const juce::RelativeTime& time);

private:
    class LookAndFeel;

    std::unique_ptr<LookAndFeel> lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stopwatch)
};

