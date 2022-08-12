#pragma once

#include "CommandMessage.h"
#include "DevicePanel/DevicePanel.h"
#include "DragOverlay.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "NetworkDiscoveryDispatcher.h"
#include "OpenGL/GLRenderer.h"

class DevicePanelContainer : public juce::Component
{
public:
    DevicePanelContainer(juce::ValueTree& windowLayout_, GLRenderer& glRenderer_);

    void resized() override;

    void connectToDevice(const ximu3::ConnectionInfo& connectionInfo);

    std::vector<DevicePanel*> getDevicePanels() const;

    void removeAllPanels();

    void removePanel(const DevicePanel& panel);

    void movePanel(DevicePanel& move, DevicePanel& target);

    void showDragOverlayAtComponent(juce::Component& component, DragOverlay::Side side);

    void hideDragOverlay();

    DragOverlay* getCurrentlyShowingDragOverlay();

    enum class Layout
    {
        rows, columns, grid, accordion
    };

    void setLayout(Layout layout_);

    Layout getLayout();

    void updateHeightInAccordionMode();

    void toggleAccordionState(DevicePanel* const devicePanel);

    std::function<void(const int oldSize, const int newSize)> onDevicePanelsSizeChanged;

private:
    juce::ValueTree& windowLayout;
    GLRenderer& glRenderer;

    std::vector<std::unique_ptr<DevicePanel>> devicePanels;
    std::unique_ptr<DragOverlay> dragOverlay;

    class AccordionResizeBar : public juce::Component
    {
    public:
        AccordionResizeBar();

        void mouseDrag(const juce::MouseEvent& mouseEvent) override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AccordionResizeBar)
    };

    AccordionResizeBar accordionResizeBar;

    Layout layout = Layout::rows;
    DevicePanel* expandedDevicePanel = nullptr;
    int expandedPanelHeight = 600;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanelContainer)
};
