#pragma once

#include "CustomLookAndFeel.h"
#include "DevicePanelContainer.h"
#include "Dialogs/Dialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "MenuStrip/MenuStrip.h"
#include "OpenGL/GLRenderer.h"
#include "Windows/WindowIDs.h"

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        addAndMakeVisible(menuStrip);
        addAndMakeVisible(devicePanelViewport);

        devicePanelViewport.setViewedComponent(&devicePanelContainer, false);
        devicePanelViewport.setScrollBarsShown(true, false);

        juce::LookAndFeel::setDefaultLookAndFeel(&lnf);
        setSize(1280, 768);

        tooltipWindow.setOpaque(false);
    }

    ~MainComponent() override
    {
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        menuStrip.setBounds(bounds.removeFromTop(72));
        bounds.removeFromTop(UILayout::panelMargin);

        devicePanelViewport.setBounds(bounds);
        devicePanelContainer.setBounds(0, 0, bounds.getWidth(), devicePanelContainer.getLayout() == DevicePanelContainer::Layout::accordion ? devicePanelContainer.getHeight() : bounds.getHeight());
    }

private:
    juce::ValueTree windowLayout { WindowIDs::Row };
    GLRenderer glRenderer { *this };
    DevicePanelContainer devicePanelContainer { windowLayout, glRenderer };
    juce::Viewport devicePanelViewport;
    MenuStrip menuStrip { windowLayout, devicePanelContainer };
    CustomLookAndFeel lnf;
    juce::TooltipWindow tooltipWindow { nullptr, 1200 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
