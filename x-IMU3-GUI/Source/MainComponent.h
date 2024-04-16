#pragma once

#include "ConnectionPanelContainer.h"
#include "CustomLookAndFeel.h"
#include "Dialogs/Dialog.h"
#include "Dialogs/MessageDialog.h"
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
            DialogQueue::getSingleton().pushFront(std::make_unique<ErrorDialog>("Unable to access the network announcement socket. Please close all other x-IMU3 applications and restart the x-IMU3 GUI."));
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
    juce::ThreadPool threadPool;
    GLRenderer glRenderer { *this, threadPool };
    ConnectionPanelContainer connectionPanelContainer { windowLayout, glRenderer, threadPool };
    juce::Viewport connectionPanelViewport;
    MenuStrip menuStrip { windowLayout, threadPool, connectionPanelContainer };
    juce::TooltipWindow tooltipWindow { nullptr, 300 };
    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
