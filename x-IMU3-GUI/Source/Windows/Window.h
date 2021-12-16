#pragma once

#include "DragOverlay.h"
#include <juce_gui_basics/juce_gui_basics.h>

class DevicePanel;

class Window : public juce::Component
{
public:
    Window(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_);

    void paint(juce::Graphics& g) override;

    void resized() override;

    juce::Rectangle<int> getContentBounds() const;

    const juce::Identifier& getType() const;

    static juce::ValueTree findWindow(const juce::ValueTree root, const juce::Identifier& type);

protected:
    DevicePanel& devicePanel;

private:
    class Title : public SimpleLabel
    {
    public:
        explicit Title(Window& parentWindow_);

        void mouseDown(const juce::MouseEvent& mouseEvent) override;

        void mouseDrag(const juce::MouseEvent& mouseEvent) override;

        void mouseUp(const juce::MouseEvent& mouseEvent) override;

    private:
        Window& parentWindow;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Title)
    };

    const juce::ValueTree& windowLayout;
    const juce::Identifier type;

    Title title { *this };

    static void removeFromParent(juce::ValueTree tree);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Window)
};
