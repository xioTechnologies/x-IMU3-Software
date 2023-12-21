#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "WindowHeader.h"

class ConnectionPanel;

class Window : public juce::Component
{
public:
    Window(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, ConnectionPanel& connectionPanel_, const juce::String& menuButtonTooltip);

    void resized() override;

    juce::Rectangle<int> getContentBounds() const;

    const juce::Identifier& getType() const;

protected:
    ConnectionPanel& connectionPanel;

    virtual juce::PopupMenu getMenu();

private:
    const juce::ValueTree& windowLayout;
    const juce::Identifier type;
    WindowHeader header;

    void closeWindow(const juce::Identifier& type_);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Window)
};
