#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "WindowHeader.h"

class ConnectionPanel;

class Window : public juce::Component,
               private juce::ValueTree::Listener
{
public:
    Window(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, const juce::String& menuButtonTooltip);

    void resized() override;

    juce::Rectangle<int> getContentBounds() const;

    const juce::Identifier& getType() const;

protected:
    juce::ValueTree settingsTree;
    ConnectionPanel& connectionPanel;

    virtual juce::PopupMenu getMenu();

private:
    juce::ValueTree windowLayout;
    const juce::Identifier type;
    WindowHeader header;

    void closeWindow(const juce::Identifier& type_);

    void valueTreeChildAdded(juce::ValueTree&, juce::ValueTree&) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Window)
};
