#pragma once

#include "CustomLookAndFeel.h"
#include "DevicePanelContainer.h"
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
        addAndMakeVisible(devicePanelViewport);

        devicePanelViewport.setViewedComponent(&devicePanelContainer, false);
        devicePanelViewport.setScrollBarsShown(true, false);

        setSize(1280, 768);

        tooltipWindow.setOpaque(false);

        if (networkAnnouncement->getResult() != ximu3::XIMU3_ResultOk)
        {
            DialogQueue::getSingleton().push(std::make_unique<ErrorDialog>("Unable to open network announcement socket."));
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        menuStrip.setBounds(bounds.removeFromTop(58));
        bounds.removeFromTop(UILayout::panelMargin);

        devicePanelViewport.setBounds(bounds);
        devicePanelContainer.setSize(bounds.getWidth(), (devicePanelContainer.getLayout() == DevicePanelContainer::Layout::accordion) ? devicePanelContainer.getHeight() : bounds.getHeight());
        devicePanelContainer.resized();
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
    DevicePanelContainer devicePanelContainer { windowLayout, glRenderer };
    juce::Viewport devicePanelViewport;
    MenuStrip menuStrip { windowLayout, devicePanelContainer };
    juce::TooltipWindow tooltipWindow { nullptr, 1200 };
    juce::SharedResourcePointer<ximu3::NetworkAnnouncement> networkAnnouncement;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
