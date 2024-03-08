#include "ApplicationSettings.h"
#include "ConnectionPanel/ConnectionPanel.h"
#include "ConnectionPanelContainer.h"
#include "Dialogs/MessageDialog.h"

ConnectionPanelContainer::ConnectionPanelContainer(juce::ValueTree& windowLayout_, GLRenderer& glRenderer_, juce::ThreadPool& threadPool_)
        : windowLayout(windowLayout_),
          glRenderer(glRenderer_),
          threadPool(threadPool_)
{
    addAndMakeVisible(noConnectionsLabel);
    addChildComponent(&accordionResizeBar);
}

void ConnectionPanelContainer::resized()
{
    noConnectionsLabel.setBounds(getLocalBounds());

    int numberOfRows = 0, numberOfColumns = 0; // initialisation to avoid compiler warning

    switch (getLayoutInternal())
    {
        case Layout::rows:
            numberOfRows = (int) connectionPanels.size();
            numberOfColumns = 1;
            break;
        case Layout::columns:
            numberOfRows = 1;
            numberOfColumns = (int) connectionPanels.size();
            break;
        case Layout::grid:
            numberOfRows = juce::roundToInt(std::sqrt(connectionPanels.size()));
            numberOfColumns = (int) std::ceil(std::sqrt(connectionPanels.size()));
            break;
        case Layout::accordion:
            for (size_t index = 0; index < connectionPanels.size(); index++)
            {
                auto& connectionPanel = connectionPanels[index];
                const auto y = (index == 0) ? 0 : (connectionPanels[index - 1]->getBottom() + UILayout::panelMargin);

                if (expandedConnectionPanel == connectionPanel.get())
                {
                    connectionPanel->setBounds(0, y, getWidth(), expandedPanelHeight);
                    accordionResizeBar.setBounds(0, connectionPanel->getBottom(), getWidth(), UILayout::panelMargin);
                }
                else
                {
                    connectionPanel->setBounds(0, y, getWidth(), ConnectionPanel::collapsedHeight);
                }
            }
            return;
    }

    auto panelsBounds = getLocalBounds().toFloat();
    const auto columnWidth = (panelsBounds.getWidth() - (numberOfColumns - 1) * UILayout::panelMargin) / numberOfColumns;
    const auto rowHeight = (panelsBounds.getHeight() - (numberOfRows - 1) * UILayout::panelMargin) / numberOfRows;

    size_t connectionPanelIndex = 0;
    int top = 0;
    for (int rowIndex = 0; rowIndex < numberOfRows; rowIndex++)
    {
        auto rowBounds = panelsBounds.removeFromTop(rowHeight).withTop((float) top);
        panelsBounds.removeFromTop(UILayout::panelMargin);

        int left = 0;
        for (int columnIndex = 0; columnIndex < numberOfColumns && connectionPanelIndex < connectionPanels.size(); columnIndex++)
        {
            const auto bounds = rowBounds.removeFromLeft(columnWidth).toNearestInt().withLeft(left);
            rowBounds.removeFromLeft(UILayout::panelMargin);

            connectionPanels[connectionPanelIndex++]->setBounds(bounds);

            left = bounds.getRight() + UILayout::panelMargin;
        }

        top = juce::roundToInt(rowBounds.getBottom()) + UILayout::panelMargin;
    }
}

void ConnectionPanelContainer::updateSize()
{
    auto* const viewport = findParentComponentOfClass<juce::Viewport>();
    if (viewport == nullptr)
    {
        return;
    }

    auto bounds = viewport->getLocalBounds();

    switch (getLayoutInternal())
    {
        case Layout::rows:
        {
            static constexpr int minimumRowHeight = 175;
            bounds.setHeight(std::max(bounds.getHeight(), (int) connectionPanels.size() * minimumRowHeight + (int) connectionPanels.size() * UILayout::panelMargin));
            break;
        }
        case Layout::columns:
        {
            static constexpr int minimumColumnWidth = 100;
            bounds.setWidth(std::max(bounds.getWidth(), (int) connectionPanels.size() * minimumColumnWidth + (int) connectionPanels.size() * UILayout::panelMargin));
            break;
        }
        case Layout::grid:
            break;
        case Layout::accordion:
            bounds.setHeight(0);
            for (size_t index = 0; index < connectionPanels.size(); index++)
            {
                bounds.setHeight(bounds.getHeight() + UILayout::panelMargin + (expandedConnectionPanel == connectionPanels[index].get() ? expandedPanelHeight : ConnectionPanel::collapsedHeight));
            }
            break;
    }

    setBounds(bounds); // Update the visibility of the scrollbars

    // Prevent scrollbar overlap
    switch (getLayoutInternal())
    {
        case Layout::rows:
            bounds.removeFromRight((bounds.getHeight() <= viewport->getMaximumVisibleHeight()) ? 0 : viewport->getScrollBarThickness());
            break;
        case Layout::columns:
            bounds.removeFromBottom((bounds.getWidth() <= viewport->getMaximumVisibleWidth()) ? 0 : viewport->getScrollBarThickness());
            break;
        case Layout::grid:
            break;
        case Layout::accordion:
            bounds.removeFromRight((bounds.getHeight() <= viewport->getMaximumVisibleHeight()) ? 0 : viewport->getScrollBarThickness());
            break;
    }

    setBounds(bounds);
    resized();
}

void ConnectionPanelContainer::connectToDevice(const ximu3::ConnectionInfo& connectionInfo)
{
    for (const auto& connectionPanel : connectionPanels)
    {
        if (connectionPanel->getConnection()->getInfo()->toString() == connectionInfo.toString())
        {
            DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Connection already exists. " + connectionInfo.toString() + "."));
            return;
        }
    }

    auto connection = std::make_shared<ximu3::Connection>(connectionInfo);
    addAndMakeVisible(*connectionPanels.emplace_back(std::make_unique<ConnectionPanel>(windowLayout, connection, glRenderer, threadPool, *this, [&]
    {
        static unsigned int counter;

        if (connectionPanels.empty() || (++counter >= UIColours::tags.size()))
        {
            counter = 0;
        }

        return UIColours::tags[counter];
    }())));

    connectionPanelsSizeChanged();
}

std::vector<ConnectionPanel*> ConnectionPanelContainer::getConnectionPanels() const
{
    std::vector<ConnectionPanel*> vector;
    for (auto& connectionPanel : connectionPanels)
    {
        vector.push_back(connectionPanel.get());
    }
    return vector;
}

void ConnectionPanelContainer::removeAllPanels()
{
    connectionPanels.clear();
    connectionPanelsSizeChanged();
}

void ConnectionPanelContainer::removePanel(const ConnectionPanel& panel)
{
    for (size_t index = 0; index < connectionPanels.size(); index++)
    {
        if (connectionPanels[index].get() == &panel)
        {
            connectionPanels.erase(connectionPanels.begin() + (int) index);
            break;
        }
    }
    connectionPanelsSizeChanged();
}

void ConnectionPanelContainer::movePanel(ConnectionPanel& move, ConnectionPanel& target)
{
    for (size_t index = 0; index < connectionPanels.size(); index++)
    {
        if (connectionPanels[index].get() == &move)
        {
            auto moving = std::move(connectionPanels[index]);
            for (; index < connectionPanels.size() - 1; index++)
            {
                connectionPanels[index] = std::move(connectionPanels[index + 1]);
                if (connectionPanels[index].get() == &target)
                {
                    connectionPanels[index + 1] = std::move(moving);
                    resized();
                    return;
                }
            }
        }
        if (connectionPanels[index].get() == &target)
        {
            auto& targetPointer = connectionPanels[index++];
            for (; index < connectionPanels.size(); index++)
            {
                std::swap(connectionPanels[index], targetPointer);
                if (targetPointer.get() == &move)
                {
                    resized();
                    return;
                }
            }
        }
    }
}

void ConnectionPanelContainer::showDragOverlayAtComponent(juce::Component& component, DragOverlay::Side side)
{
    if (dragOverlay == nullptr || dragOverlay->getTarget() != &component)
    {
        dragOverlay.reset();
        dragOverlay = std::make_unique<DragOverlay>(component);
        component.addAndMakeVisible(*dragOverlay);
    }

    dragOverlay->setSide(side);
}

void ConnectionPanelContainer::hideDragOverlay()
{
    dragOverlay.reset();
}

DragOverlay* ConnectionPanelContainer::getCurrentlyShowingDragOverlay()
{
    return dragOverlay.get();
}

void ConnectionPanelContainer::setLayout(Layout layout_)
{
    if (layout == layout_)
    {
        return;
    }

    layout = layout_;

    accordionResizeBar.setVisible(getLayoutInternal() == Layout::accordion);
    setExpandedConnectionPanel(expandedConnectionPanel);

    updateSize();
}

ConnectionPanelContainer::Layout ConnectionPanelContainer::getLayout()
{
    return layout;
}

void ConnectionPanelContainer::setExpandedConnectionPanel(ConnectionPanel* const connectionPanel)
{
    expandedConnectionPanel = connectionPanel;

    if (expandedConnectionPanel == nullptr || getLayoutInternal() != Layout::accordion)
    {
        for (auto& panel : connectionPanels)
        {
            panel->setAlpha(1.0f);
        }
    }
    else
    {
        for (auto& panel : connectionPanels)
        {
            panel->setAlpha(0.5f);
        }
        expandedConnectionPanel->setAlpha(1.0f);
    }

    updateSize();
}

const ConnectionPanel* ConnectionPanelContainer::getExpandedConnectionPanel() const
{
    return expandedConnectionPanel;
}

ConnectionPanelContainer::AccordionResizeBar::AccordionResizeBar()
{
    setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
}

void ConnectionPanelContainer::AccordionResizeBar::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    auto* const connectionPanelContainer = static_cast<ConnectionPanelContainer*>(getParentComponent());
    const auto newHeight = mouseEvent.getEventRelativeTo(connectionPanelContainer->expandedConnectionPanel).getPosition().getY() - getHeight() / 2;
    connectionPanelContainer->expandedPanelHeight = juce::jmax(ConnectionPanel::collapsedHeight, newHeight);
    connectionPanelContainer->updateSize();
}

ConnectionPanelContainer::Layout ConnectionPanelContainer::getLayoutInternal() const
{
    if (connectionPanels.size() <= 1)
    {
        return Layout::rows;
    }
    return layout;
}

void ConnectionPanelContainer::connectionPanelsSizeChanged()
{
    noConnectionsLabel.setVisible(connectionPanels.empty());
    setExpandedConnectionPanel(expandedConnectionPanel);
    onConnectionPanelsSizeChanged();
}
