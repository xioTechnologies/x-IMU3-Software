#include "TerminalFeed.h"

TerminalFeed::TerminalFeed(const juce::Colour& backgroundColour_) : backgroundColour(backgroundColour_)
{
    addAndMakeVisible(scrollbar);
    scrollbar.addListener(this);
}

void TerminalFeed::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);
    g.setFont(font);

    const auto textWidth = getWidth() - scrollbar.getWidth();
    const auto firstLineOnScreen = juce::jmin(juce::roundToInt(scrollbar.getCurrentRangeStart()),
                                              juce::jmax(0, (int) lines.size() - numberOfLinesOnScreen));
    const auto lastLineOnScreen = juce::jmin(firstLineOnScreen + numberOfLinesOnScreen, (int) lines.size());

    int y = 0;
    for (int index = firstLineOnScreen; index < lastLineOnScreen; index++)
    {
        g.setColour(lines[(size_t) index].colour);
        g.drawText(lines[(size_t) index].message, 0, y, textWidth, lineHeight, juce::Justification::centredLeft, false);
        y += lineHeight;
    }
}

void TerminalFeed::mouseDown(const juce::MouseEvent& mouseEvent)
{
    if (mouseEvent.mods.isPopupMenu())
    {
        juce::PopupMenu menu;
        menu.addItem("Copy To Clipboard", [this]
        {
            juce::String text;
            for (const auto& message : messages)
            {
                text += message.message + "\n";
            }
            juce::SystemClipboard::copyTextToClipboard(text);
        });
        menu.addItem("Clear All", [this]
        {
            clear();
        });
        menu.showAt({ mouseEvent.getMouseDownScreenX(), mouseEvent.getMouseDownScreenY() - 10, 10, 10 });
    }
}

void TerminalFeed::mouseWheelMove(const juce::MouseEvent& mouseEvent, const juce::MouseWheelDetails& wheel)
{
    auto wheelCopy = wheel;
    wheelCopy.deltaY *= 15.0f;
    scrollbar.mouseWheelMove(mouseEvent, wheelCopy);
}

void TerminalFeed::resized()
{
    scrollbar.setBounds(getLocalBounds().removeFromRight(10));
    numberOfLinesOnScreen = getHeight() / lineHeight;
    numberOfCharactersPerLine = (int) (scrollbar.getX() / characterWidth);
    numberOfCharactersPerWrappedLine = juce::jmax(1, numberOfCharactersPerLine - wrapIndentation.length());

    if (numberOfCharactersPerLine == 0)
    {
        return;
    }

    lines.clear();
    for (auto& message : messages)
    {
        for (auto& line : wrapped(message))
        {
            lines.push_back(line);
        }
    }
    updateScrollbar();
}

void TerminalFeed::add(const juce::String& label, const juce::String& text, const juce::Colour& colour)
{
    const Item item({ "[" + label.paddedLeft(' ', label.length() + (labelTemplate.length() - label.length()) / 2)
                                 .paddedRight(' ', labelTemplate.length())
                                 .substring(0, labelTemplate.length()) + "] " + text, colour });
    messages.push_back(item);
    if (numberOfCharactersPerLine == 0)
    {
        return;
    }

    for (auto& line : wrapped(item))
    {
        lines.push_back(line);
    }

    if ((int) messages.size() > maxNumberOfMessages)
    {
        const auto numberOfLinesToDrop = (int64_t) wrapped(messages[0]).size();
        messages.erase(messages.begin());
        lines.erase(lines.begin(), lines.begin() + numberOfLinesToDrop);
    }

    updateScrollbar();
}

void TerminalFeed::add(const juce::Time& time, const juce::String& text, const juce::Colour& colour)
{
    add(time.formatted("%I:%M:%S"), text, colour);
}

void TerminalFeed::add(const uint64_t timestamp, const juce::String& text, const juce::Colour& colour)
{
    add(juce::String(timestamp / 1'000'000.0f, labelTemplate.length()), text, colour);
}

void TerminalFeed::applyColourToAll(const juce::Colour& colour)
{
    for (auto& message : messages)
    {
        message.colour = colour;
    }
    for (auto& line : lines)
    {
        line.colour = colour;
    }
}

void TerminalFeed::clear()
{
    messages.clear();
    lines.clear();
    updateScrollbar();
    if (onClear != nullptr)
    {
        onClear();
    }
}

void TerminalFeed::updateScrollbar()
{
    const auto wasScrolledDown = juce::roundToInt(scrollbar.getCurrentRange().getEnd()) >= scrollbar.getMaximumRangeLimit();
    scrollbar.setRangeLimits({ 0.0, (double) lines.size() }, juce::dontSendNotification);
    scrollbar.setCurrentRange(wasScrolledDown ? (scrollbar.getMaximumRangeLimit() - (double) numberOfLinesOnScreen) : scrollbar.getCurrentRangeStart(), (double) numberOfLinesOnScreen, juce::dontSendNotification);
    repaint();
}

std::vector<TerminalFeed::Item> TerminalFeed::wrapped(const Item& item) const
{
    std::vector<Item> result;
    result.push_back({ item.message.substring(0, numberOfCharactersPerLine), item.colour });
    for (int index = numberOfCharactersPerLine; index < item.message.length(); index += numberOfCharactersPerWrappedLine)
    {
        result.push_back({ wrapIndentation + item.message.substring(index, index + numberOfCharactersPerWrappedLine), item.colour });
    }
    return result;
}

void TerminalFeed::scrollBarMoved(juce::ScrollBar*, double)
{
    repaint();
}
