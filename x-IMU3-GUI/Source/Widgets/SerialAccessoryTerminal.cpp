#include "EscapedStrings.h"
#include "SerialAccessoryTerminal.h"

SerialAccessoryTerminal::SerialAccessoryTerminal()
{
    addAndMakeVisible(scrollbar);
    scrollbar.addListener(this);

    setOpaque(true);
}

void SerialAccessoryTerminal::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    for (auto lineIndex = (int) std::floor(scrollbar.getCurrentRangeStart()); lineIndex < (int) std::ceil(scrollbar.getCurrentRange().getEnd()); lineIndex++)
    {
        const auto y = juce::jmap((float) lineIndex, (float) scrollbar.getCurrentRangeStart(), (float) scrollbar.getCurrentRangeStart() + (float) numberOfLinesOnScreen, 0.0f, (float) getHeight());
        wrappedLines[(size_t) lineIndex].draw(g, { 0.0f, (float) y, (float) getWidth(), (float) juce::roundToInt(font.getHeight()) });
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
    numberOfCharactersPerLine = std::max(1, (int) std::floor(scrollbar.getX() / juce::GlyphArrangement::getStringWidth(font, "0")));

    wrappedLines.clear();
    for (const auto& line : lines)
    {
        for (const auto& wrappedLine : wrapped(line))
        {
            wrappedLines.push_back(wrappedLine);
        }
    }

    updateScrollbarRange();
}

void SerialAccessoryTerminal::addRx(const uint64_t timestamp, const juce::String& text)
{
    juce::AttributedString line;
    line.append(juce::String(1E-6f * (float) timestamp, 3) + " ", juce::Colours::grey);
    for (const auto& string : EscapedStrings::splitPrintable(text.toStdString()))
    {
        line.append(string, string.starts_with("\\") ? juce::Colours::grey : juce::Colours::white);
    }
    addLine(line);
}

void SerialAccessoryTerminal::addTx(const juce::String& text)
{
    juce::AttributedString line;
    line.append("TX ", UIColours::success);
    for (const auto& string : EscapedStrings::splitPrintable(text.toStdString()))
    {
        line.append(string, string.starts_with("\\") ? juce::Colours::grey : juce::Colours::white);
    }
    addLine(line);
}

void SerialAccessoryTerminal::addError(const juce::String& text)
{
    juce::AttributedString line;
    line.append("Invalid JSON \"" + text + "\"", UIColours::error);
    addLine(line);
}

void SerialAccessoryTerminal::copyToClipboard() const
{
    juce::String text;
    for (const auto& line : lines)
    {
        text += line.getText() + "\n";
    }
    juce::SystemClipboard::copyTextToClipboard(text);
}

void SerialAccessoryTerminal::clearAll()
{
    lines.clear();
    wrappedLines.clear();
    updateScrollbarRange();
}

void SerialAccessoryTerminal::addLine(const juce::AttributedString& line)
{
    lines.push_back(line);

    const auto wrappedLine = wrapped(line);
    wrappedLines.insert(wrappedLines.end(), wrappedLine.begin(), wrappedLine.end());

    if ((int) lines.size() > maxNumberOfLines)
    {
        const auto numberOfLinesToDrop = (int) wrapped(lines[0]).size();
        lines.erase(lines.begin());
        wrappedLines.erase(wrappedLines.begin(), wrappedLines.begin() + numberOfLinesToDrop);
    }

    updateScrollbarRange();
}

void SerialAccessoryTerminal::updateScrollbarRange()
{
    const auto wasScrolledDown = juce::roundToInt(scrollbar.getCurrentRange().getEnd()) >= scrollbar.getMaximumRangeLimit();
    scrollbar.setRangeLimits({ 0.0, (double) wrappedLines.size() }, juce::dontSendNotification);
    scrollbar.setCurrentRange(wasScrolledDown ? (scrollbar.getMaximumRangeLimit() - (double) numberOfLinesOnScreen) : scrollbar.getCurrentRangeStart(), (double) numberOfLinesOnScreen, juce::dontSendNotification);
    repaint();
}

std::vector<juce::AttributedString> SerialAccessoryTerminal::wrapped(const juce::AttributedString& line) const
{
    std::vector<juce::AttributedString> wrappedLine;

    int attributeIndex = 0, characterStartIndex = 0, lineEndIndex = 0;
    while (attributeIndex < line.getNumAttributes())
    {
        if (characterStartIndex == lineEndIndex)
        {
            wrappedLine.push_back({});
            lineEndIndex += numberOfCharactersPerLine;
        }

        const auto& attribute = line.getAttribute(attributeIndex);
        const auto characterEndIndex = std::min(lineEndIndex, attribute.range.getEnd());

        wrappedLine.back().append(line.getText().substring(characterStartIndex, characterEndIndex), font, attribute.colour);

        characterStartIndex = characterEndIndex;
        if (characterStartIndex == attribute.range.getEnd())
        {
            attributeIndex++;
        }
    }

    return wrappedLine;
}

void SerialAccessoryTerminal::scrollBarMoved(juce::ScrollBar*, double)
{
    repaint();
}
