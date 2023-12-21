#pragma once

#include "ConnectionPanelContainer.h"
#include "CustomLookAndFeel.h"
#include "Dialogs/Dialog.h"
#include "Dialogs/ErrorDialog.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "MenuStrip/MenuStrip.h"
#include "OpenGL/Common/GLRenderer.h"
#include "Windows/WindowIDs.h"

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        addAndMakeVisible(menuStrip);
        addAndMakeVisible(connectionPanelViewport);

        connectionPanelViewport.setViewedComponent(&connectionPanelContainer, false);
        connectionPanelViewport.setScrollBarsShown(true, true);

        setSize(1280, 768);

        tooltipWindow.setOpaque(false);

        if (networkAnnouncement->getResult() != ximu3::XIMU3_ResultOk)
        {
            DialogQueue::getSingleton().pushFront(std::make_unique<ErrorDialog>("Unable to open network announcement socket."));
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        menuStrip.setBounds(bounds.removeFromTop(40));
        bounds.removeFromTop(UILayout::panelMargin);

        connectionPanelViewport.setBounds(bounds);
        connectionPanelContainer.updateSize();
    }

    int getMinimumWidth() const
    {
        return menuStrip.getMinimumWidth();
    }

    int getMinimumHeight() const
    {
        return juce::roundToInt(menuStrip.getMinimumWidth() / (1024.0f / 768.0f));
    }

private:
    struct DefaultLookAndFeelSetter
    {
        DefaultLookAndFeelSetter()
        {
            juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);
        }

        CustomLookAndFeel lookAndFeel;
    };

    const DefaultLookAndFeelSetter defaultLookAndFeelSetter;
    juce::ValueTree windowLayout { WindowIDs::Row };
    GLRenderer glRenderer { *this };
    ConnectionPanelContainer connectionPanelContainer { windowLayout, glRenderer };
    juce::Viewport connectionPanelViewport;
    MenuStrip menuStrip { windowLayout, connectionPanelContainer };
    juce::TooltipWindow tooltipWindow { nullptr, 1200 };
    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
