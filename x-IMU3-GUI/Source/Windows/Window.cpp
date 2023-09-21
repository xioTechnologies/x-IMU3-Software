#include "Window.h"

Window::Window(const juce::ValueTree& windowLayout, const juce::Identifier& type_, DevicePanel& devicePanel_, const juce::String& menuButtonTooltip, std::function<juce::PopupMenu()> getPopup)
        : devicePanel(devicePanel_),
          type(type_),
          header(devicePanel_, windowLayout, type_, menuButtonTooltip, getPopup)
{
    addAndMakeVisible(header);
}

void Window::resized()
{
    header.setBounds(getLocalBounds().removeFromTop(22));
}

juce::Rectangle<int> Window::getContentBounds() const
{
    return getLocalBounds().withTrimmedTop(header.getHeight());
}

const juce::Identifier& Window::getType() const
{
    return type;
}
