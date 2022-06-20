#include "../Helpers.h"
#include "SerialAccessoryTerminal.h"

SerialAccessoryTerminal::SerialAccessoryTerminal()
{
    addAndMakeVisible(scrollbar);
    scrollbar.addListener(this);
}

void SerialAccessoryTerminal::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    const auto firstLineOnScreen = juce::jmin(juce::roundToInt(scrollbar.getCurrentRangeStart()), juce::jmax(0, (int) wrappedMessages.size() - numberOfLinesOnScreen));

    int y = 0;
    for (int index = firstLineOnScreen; index < juce::jmin(firstLineOnScreen + numberOfLinesOnScreen, (int) wrappedMessages.size()); index++)
    {
        wrappedMessages[(size_t) index].draw(g, { 0.0f, (float) y, (float) getWidth(), (float) juce::roundToInt(font.getHeight()) });
        y += juce::roundToInt(font.getHeight());
    }
}

void SerialAccessoryTerminal::mouseDown(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.mods.isPopupMenu())
    {
        juce::PopupMenu menu;
        menu.addItem("Copy To Clipboard", [this]
        {
            juce::String text;
            for (const auto& message : messages)
            {
                text += message.getText() + "\n";
            }
            juce::SystemClipboard::copyTextToClipboard(text);
        });
        menu.addItem("Clear All", [this]
        {
            messages.clear();
            wrappedMessages.clear();
            updateScrollbarRange();
        });
        menu.showAt({ mouseEvent.getMouseDownScreenX(), mouseEvent.getMouseDownScreenY() - 10, 10, 10 });
    }
}

void SerialAccessoryTerminal::mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel)
{
    auto wheelCopy = wheel;
    wheelCopy.deltaY *= 15.0f;
    scrollbar.mouseWheelMove(mouseEvent, wheelCopy);
}

void SerialAccessoryTerminal::resized()
{
    scrollbar.setBounds(getLocalBounds().removeFromRight(10));
    numberOfLinesOnScreen = (int) std::ceil(getHeight() / font.getHeight());
    numberOfCharactersPerLine = (int) std::floor(scrollbar.getX() / font.getStringWidthFloat("0"));

    wrappedMessages.clear();
    for (const auto& message : messages)
    {
        for (const auto& line : wrapped(message))
        {
            wrappedMessages.push_back(line);
        }
    }

    updateScrollbarRange();
}

void SerialAccessoryTerminal::add(const uint64_t timestamp, const juce::String& text)
{
    juce::AttributedString message;
    if (timestamp == uint64_t(-1))
    {
        message.append("TX ", juce::Colours::green);
    }
    else
    {
        message.append(Helpers::formatTimestamp(timestamp) + " ", juce::Colours::grey);
    }
    for (const auto& string : Helpers::addEscapeCharacters(text))
    {
        message.append(string, string.startsWith("\\") ? juce::Colours::grey : juce::Colours::white);
    }
    messages.push_back(message);

    const auto wrappedMessage = wrapped(message);
    wrappedMessages.insert(wrappedMessages.end(), wrappedMessage.begin(), wrappedMessage.end());

    if ((int) messages.size() > maxNumberOfMessages)
    {
        const auto numberOfLinesToDrop = (int) wrapped(messages[0]).size();
        messages.erase(messages.begin());
        wrappedMessages.erase(wrappedMessages.begin(), wrappedMessages.begin() + numberOfLinesToDrop);
    }

    updateScrollbarRange();
}

void SerialAccessoryTerminal::updateScrollbarRange()
{
    const auto wasScrolledDown = juce::roundToInt(scrollbar.getCurrentRange().getEnd()) >= scrollbar.getMaximumRangeLimit();
    scrollbar.setRangeLimits({ 0.0, (double) wrappedMessages.size() }, juce::dontSendNotification);
    scrollbar.setCurrentRange(wasScrolledDown ? (scrollbar.getMaximumRangeLimit() - (double) numberOfLinesOnScreen) : scrollbar.getCurrentRangeStart(), (double) numberOfLinesOnScreen, juce::dontSendNotification);
    repaint();
}

std::vector<juce::AttributedString> SerialAccessoryTerminal::wrapped(const juce::AttributedString& message) const
{
    std::vector<juce::AttributedString> wrappedMessage;

    int attributeIndex = 0, characterStartIndex = 0, lineEndIndex = 0;
    while (attributeIndex < message.getNumAttributes())
    {
        if (characterStartIndex == lineEndIndex)
        {
            wrappedMessage.push_back({});
            lineEndIndex += numberOfCharactersPerLine;
        }

        const auto& attribute = message.getAttribute(attributeIndex);
        const auto characterEndIndex = std::min(lineEndIndex, attribute.range.getEnd());

        wrappedMessage.back().append(message.getText().substring(characterStartIndex, characterEndIndex), font, attribute.colour);

        characterStartIndex = characterEndIndex;
        if (characterStartIndex == attribute.range.getEnd())
        {
            attributeIndex++;
        }
    }

    return wrappedMessage;
}

void SerialAccessoryTerminal::scrollBarMoved(juce::ScrollBar*, double)
{
    repaint();
}
