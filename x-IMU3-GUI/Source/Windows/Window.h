#pragma once

#include "DragOverlay.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Widgets/IconButton.h"

class DevicePanel;

class Window : public juce::Component
{
public:
    Window(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_);

    void resized() override;

    juce::Rectangle<int> getContentBounds() const;

    const juce::Identifier& getType() const;

    static juce::ValueTree findWindow(const juce::ValueTree root, const juce::Identifier& type);

protected:
    DevicePanel& devicePanel;

    virtual juce::PopupMenu getMenu() = 0;

private:
    class Header : public juce::Component
    {
    public:
        explicit Header(Window& window_);

        void paint(juce::Graphics& g) override;

        void resized() override;

        void mouseDown(const juce::MouseEvent& mouseEvent) override;

        void mouseDrag(const juce::MouseEvent& mouseEvent) override;

        void mouseUp(const juce::MouseEvent& mouseEvent) override;

    private:
        Window& window;

        IconButton menuButton { BinaryData::menu_svg, "Window Menu", std::bind(&Window::getMenu, &window), false };
        SimpleLabel title { window.getName(), UIFonts::getSmallFont(), juce::Justification::centred };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Header)
    };

    const juce::ValueTree& windowLayout;
    const juce::Identifier type;

    Header header { *this };

    static void removeFromParent(juce::ValueTree tree);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Window)
};
