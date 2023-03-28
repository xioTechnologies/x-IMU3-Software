#include "../Convert.h"
#include "SerialAccessoryTerminal.h"

SerialAccessoryTerminal::SerialAccessoryTerminal()
{
    addAndMakeVisible(scrollbar);
    scrollbar.addListener(this);
}

void SerialAccessoryTerminal::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    for (auto lineIndex = (int) std::floor(scrollbar.getCurrentRangeStart()); lineIndex < (int) std::ceil(scrollbar.getCurrentRange().getEnd()); lineIndex++)
    {
        const auto y = juce::jmap((float) lineIndex, (float) scrollbar.getCurrentRangeStart(), (float) scrollbar.getCurrentRangeStart() + (float) numberOfLinesOnScreen, 0.0f, (float) getHeight());
        wrappedMessages[(size_t) lineIndex].draw(g, { 0.0f, (float) y, (float) getWidth(), (float) juce::roundToInt(font.getHeight()) });
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
    numberOfLinesOnScreen = getHeight() / font.getHeight();
    numberOfCharactersPerLine = std::max(1, (int) std::floor(scrollbar.getX() / font.getStringWidthFloat("0")));

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
        message.append(juce::String(1E-6f * (float) timestamp, 3) + " ", juce::Colours::grey);
    }
    for (const auto& string : addEscapeCharacters(text))
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

void SerialAccessoryTerminal::copyToClipboard() const
{
    juce::String text;
    for (const auto& message : messages)
    {
        text += message.getText() + "\n";
    }
    juce::SystemClipboard::copyTextToClipboard(text);
}

void SerialAccessoryTerminal::clearAll()
{
    messages.clear();
    wrappedMessages.clear();
    updateScrollbarRange();
}

std::vector<juce::String> SerialAccessoryTerminal::addEscapeCharacters(const juce::String& input)
{
    std::vector<juce::String> output(1);

    for (const auto character : input)
    {
        if (juce::CharacterFunctions::isPrintable(character))
        {
            if (output.back()[0] == '\\')
            {
                output.push_back({});
            }

            output.back() += character;
            continue;
        }

        if (output.back().isNotEmpty())
        {
            output.push_back({});
        }

        switch (character)
        {
            case '\\':
                output.back() += "\\\\";
                break;

            case '\n':
                output.back() += "\\n";
                break;

            case '\r':
                output.back() += "\\r";
                break;

            default:
                output.back() += "\\x" + juce::String::toHexString(character).paddedLeft('0', 2);
                break;
        }
    }

    return output;
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
