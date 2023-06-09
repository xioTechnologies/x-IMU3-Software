#pragma once

#include "CommandMessage.h"
#include "DevicePanel/DevicePanel.h"
#include "DragOverlay.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "OpenGL/Common/GLRenderer.h"

class DevicePanelContainer : public juce::Component
{
public:
    DevicePanelContainer(juce::ValueTree& windowLayout_, GLRenderer& glRenderer_);

    void resized() override;

    void updateSize();

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
        single,
        rows,
        columns,
        grid,
        accordion,
    };

    void setLayout(Layout layout_);

    Layout getLayout();

    void setExpandedDevicePanel(DevicePanel* const devicePanel);

    const DevicePanel* getExpandedDevicePanel() const;

    std::function<void()> onDevicePanelsSizeChanged;

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

    Layout layout = Layout::single;
    SafePointer<DevicePanel> expandedDevicePanel;
    int expandedPanelHeight = 600;

    void devicePanelsSizeChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DevicePanelContainer)
};
