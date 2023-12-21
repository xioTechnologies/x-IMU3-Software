#include "Window.h"
#include "WindowIDs.h"

Window::Window(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, const juce::String& menuButtonTooltip)
        : connectionPanel(connectionPanel_),
          windowLayout(windowLayout_),
          type(type_),
          header(connectionPanel_, windowLayout, type_, menuButtonTooltip, std::bind(&Window::getMenu, this))
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

juce::PopupMenu Window::getMenu()
{
    juce::PopupMenu menu;
    menu.addItem("Close", [&]
    {
        closeWindow(type);
    });
    menu.addItem("Close Other Windows", [&]
    {
        for (const auto& [windowType, _] : windowTitles)
        {
            if (windowType != type)
            {
                closeWindow(windowType);
            }
        }
    });
    return menu;
}

void Window::closeWindow(const juce::Identifier& type_)
{
    for (auto child = findWindow(windowLayout, type_); child.isValid() && child.getNumChildren() == 0;)
    {
        auto parent = child.getParent();
        parent.removeChild(child, nullptr);
        child = parent;
    }
}
