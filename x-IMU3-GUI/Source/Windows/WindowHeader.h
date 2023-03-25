#pragma once

#include "../Widgets/IconButton.h"
#include "../Widgets/SimpleLabel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class DevicePanel;

class WindowHeader : public juce::Component
{
public:
    explicit WindowHeader(DevicePanel& devicePanel_, const juce::ValueTree& windowLayout_, const juce::Identifier& type_, const juce::String& menuButtonTooltip, std::function<juce::PopupMenu()> getPopup);

    void paint(juce::Graphics& g) override;

    void resized() override;

    void mouseDown(const juce::MouseEvent& mouseEvent) override;

    void mouseDrag(const juce::MouseEvent& mouseEvent) override;

    void mouseUp(const juce::MouseEvent& mouseEvent) override;

private:
    DevicePanel& devicePanel;
    const juce::ValueTree& windowLayout;
    const juce::Identifier type;
    IconButton menuButton;
    SimpleLabel title;

    static void removeFromParent(juce::ValueTree tree);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WindowHeader)
};
