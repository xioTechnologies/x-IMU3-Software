#pragma once

#include "CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class SerialAccessoryTerminal : public juce::Component,
                                private juce::ScrollBar::Listener
{
public:
    static constexpr int maxNumberOfLines = 1024;

    SerialAccessoryTerminal();

    void paint(juce::Graphics& g) override;

    void mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel) override;

    void resized() override;

    void addRX(const uint64_t timestamp, const juce::String& text);

    void addTX(const juce::String& text);

    void addError(const juce::String& text);

    void copyToClipboard() const;

    void clearAll();

private:
    const juce::Font font = UIFonts::getTerminalFeedFont();

    juce::ScrollBar scrollbar { true };

    std::vector<juce::AttributedString> lines, wrappedLines;
    float numberOfLinesOnScreen = 0.0f;
    int numberOfCharactersPerLine = 1;

    static juce::StringArray splitEscapedString(const std::string& input);

    void addLine(const juce::AttributedString& line);

    void updateScrollbarRange();

    std::vector<juce::AttributedString> wrapped(const juce::AttributedString& line) const;

    void scrollBarMoved(juce::ScrollBar*, double) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialAccessoryTerminal)
};
