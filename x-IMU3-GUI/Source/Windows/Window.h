#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "WindowHeader.h"

class DevicePanel;

class Window : public juce::Component
{
public:
    Window(DevicePanel& devicePanel_, const juce::ValueTree& windowLayout, const juce::Identifier& type_, const juce::String& menuButtonTooltip, std::function<juce::PopupMenu()> getPopup);

    void resized() override;

    juce::Rectangle<int> getContentBounds() const;

    const juce::Identifier& getType() const;

protected:
    DevicePanel& devicePanel;

private:
    const juce::Identifier type;
    WindowHeader header;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Window)
};
