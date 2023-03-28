#pragma once

#include "../CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class SerialAccessoryTerminal : public juce::Component,
                                private juce::ScrollBar::Listener
{
public:
    static constexpr int maxNumberOfMessages = 1024;

    SerialAccessoryTerminal();

    void paint(juce::Graphics& g) override;

    void mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel) override;

    void resized() override;

    void add(const uint64_t timestamp, const juce::String& text);

    void copyToClipboard() const;

    void clearAll();

private:
    const juce::Font font = UIFonts::getTerminalFeedFont();

    juce::ScrollBar scrollbar { true };

    std::vector<juce::AttributedString> messages, wrappedMessages;
    float numberOfLinesOnScreen = 0.0f;
    int numberOfCharactersPerLine = 0;

    static std::vector<juce::String> addEscapeCharacters(const juce::String& input);

    void updateScrollbarRange();

    std::vector<juce::AttributedString> wrapped(const juce::AttributedString& item) const;

    void scrollBarMoved(juce::ScrollBar*, double) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SerialAccessoryTerminal)
};
