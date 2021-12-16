#pragma once

#include "../CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class TerminalFeed : public juce::Component,
                     private juce::ScrollBar::Listener
{
public:
    static constexpr int maxNumberOfMessages = 1024;

    explicit TerminalFeed(const juce::Colour& backgroundColour_ = UIColours::background);

    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

    void mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel) override;

    void resized() override;

    void add(const juce::String& label, const juce::String& text, const juce::Colour& colour);

    void add(const juce::Time& time, const juce::String& text, const juce::Colour& colour);

    void add(const uint64_t timestamp, const juce::String& text, const juce::Colour& colour);

    void applyColourToAll(const juce::Colour& colour);

    void clear();

    std::function<void()> onClear;

private:
    const juce::Colour backgroundColour;
    juce::ScrollBar scrollbar { true };

    struct Item
    {
        juce::String message;
        juce::Colour colour;
    };
    std::vector<Item> messages;
    std::vector<Item> lines;

    const juce::Font font = UIFonts::terminalFeedFont;
    const float characterWidth = font.getStringWidthFloat("0");
    const int lineHeight = juce::roundToInt(font.getHeight());

    int numberOfLinesOnScreen = 0;
    int numberOfCharactersPerLine = 0;
    int numberOfCharactersPerWrappedLine = 0;

    const juce::String labelTemplate = "hh:mm:ss";
    const juce::String wrapIndentation = juce::String::repeatedString(" ", 3 + labelTemplate.length());

    void updateScrollbar();

    std::vector<Item> wrapped(const Item& item) const;

    void scrollBarMoved(juce::ScrollBar*, double) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TerminalFeed)
};
