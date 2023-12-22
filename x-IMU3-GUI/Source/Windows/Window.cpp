#include "Window.h"
#include "WindowIDs.h"

Window::Window(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, const juce::String& menuButtonTooltip)
        : settingsTree(findWindow(windowLayout_, type_)),
          connectionPanel(connectionPanel_),
          windowLayout(windowLayout_),
          type(type_),
          header(connectionPanel_, windowLayout, type_, menuButtonTooltip, std::bind(&Window::getMenu, this))
{
    addAndMakeVisible(header);

    windowLayout.addListener(this);
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

void Window::valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&)
{
    const auto newSettingsTree = findWindow(windowLayout, type);
    if (newSettingsTree.isValid() == false)
    {
        return;
    }

    std::set<juce::Identifier> propertiesNames;
    for (auto index = 0; index < settingsTree.getNumProperties(); index++)
    {
        propertiesNames.insert(settingsTree.getPropertyName(index));
    }
    for (auto index = 0; index < newSettingsTree.getNumProperties(); index++)
    {
        propertiesNames.insert(newSettingsTree.getPropertyName(index));
    }

    settingsTree = newSettingsTree;
    for (auto property : propertiesNames)
    {
        settingsTree.sendPropertyChangeMessage(property);
    }
}
