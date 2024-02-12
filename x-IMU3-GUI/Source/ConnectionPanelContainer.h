#pragma once

#include "CommandMessage.h"
#include "ConnectionPanel/ConnectionPanel.h"
#include "DragOverlay.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "OpenGL/Common/GLRenderer.h"

class ConnectionPanelContainer : public juce::Component
{
public:
    ConnectionPanelContainer(juce::ValueTree& windowLayout_, GLRenderer& glRenderer_);

    void resized() override;

    void updateSize();

    void connectToDevice(const ximu3::ConnectionInfo& connectionInfo);

    std::vector<ConnectionPanel*> getConnectionPanels() const;

    void removeAllPanels();

    void removePanel(const ConnectionPanel& panel);

    void movePanel(ConnectionPanel& move, ConnectionPanel& target);

    void showDragOverlayAtComponent(juce::Component& component, DragOverlay::Side side);

    void hideDragOverlay();

    DragOverlay* getCurrentlyShowingDragOverlay();

    enum class Layout
    {
        rows,
        columns,
        grid,
        accordion,
    };

    void setLayout(Layout layout_);

    Layout getLayout();

    void setExpandedConnectionPanel(ConnectionPanel* const connectionPanel);

    const ConnectionPanel* getExpandedConnectionPanel() const;

    std::function<void()> onConnectionPanelsSizeChanged;

private:
    juce::ValueTree& windowLayout;
    GLRenderer& glRenderer;

    SimpleLabel noConnectionsLabel { "No Connections", UIFonts::getDefaultFont(), juce::Justification::centred };

    std::vector<std::unique_ptr<ConnectionPanel>> connectionPanels;
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

    Layout layout = Layout::grid;
    SafePointer<ConnectionPanel> expandedConnectionPanel;
    int expandedPanelHeight = 600;

    Layout getLayoutInternal() const;

    void connectionPanelsSizeChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionPanelContainer)
};
