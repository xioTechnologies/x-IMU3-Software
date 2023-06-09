#include "ApplicationSettings.h"
#include "DevicePanel/DevicePanel.h"
#include "DevicePanelContainer.h"
#include "Dialogs/ErrorDialog.h"

DevicePanelContainer::DevicePanelContainer(juce::ValueTree& windowLayout_, GLRenderer& glRenderer_)
        : windowLayout(windowLayout_),
          glRenderer(glRenderer_)
{
    addChildComponent(&accordionResizeBar);
}

void DevicePanelContainer::resized()
{
    int numberOfRows = 0, numberOfColumns = 0; // initialisation to avoid compiler warning

    switch (layout)
    {
        case Layout::single:
            numberOfRows = (int) devicePanels.size();
            numberOfColumns = (int) devicePanels.size();
            break;
        case Layout::rows:
            numberOfRows = (int) devicePanels.size();
            numberOfColumns = 1;
            break;
        case Layout::columns:
            numberOfRows = 1;
            numberOfColumns = (int) devicePanels.size();
            break;
        case Layout::grid:
            numberOfRows = juce::roundToInt(std::sqrt(devicePanels.size()));
            numberOfColumns = (int) std::ceil(std::sqrt(devicePanels.size()));
            break;
        case Layout::accordion:
            for (size_t index = 0; index < devicePanels.size(); index++)
            {
                auto& devicePanel = devicePanels[index];
                const auto y = (index == 0) ? 0 : (devicePanels[index - 1]->getBottom() + UILayout::panelMargin);

                if (expandedDevicePanel == devicePanel.get())
                {
                    devicePanel->setBounds(0, y, getWidth(), expandedPanelHeight);
                    accordionResizeBar.setBounds(0, devicePanel->getBottom(), getWidth(), UILayout::panelMargin);
                }
                else
                {
                    devicePanel->setBounds(0, y, getWidth(), DevicePanel::collapsedHeight);
                }
            }
            return;
    }

    auto panelsBounds = getLocalBounds().toFloat();
    const auto columnWidth = (panelsBounds.getWidth() - (numberOfColumns - 1) * UILayout::panelMargin) / numberOfColumns;
    const auto rowHeight = (panelsBounds.getHeight() - (numberOfRows - 1) * UILayout::panelMargin) / numberOfRows;

    size_t devicePanelIndex = 0;
    int top = 0;
    for (int rowIndex = 0; rowIndex < numberOfRows; rowIndex++)
    {
        auto rowBounds = panelsBounds.removeFromTop(rowHeight).withTop((float) top);
        panelsBounds.removeFromTop(UILayout::panelMargin);

        int left = 0;
        for (int columnIndex = 0; columnIndex < numberOfColumns && devicePanelIndex < devicePanels.size(); columnIndex++)
        {
            const auto bounds = rowBounds.removeFromLeft(columnWidth).toNearestInt().withLeft(left);
            rowBounds.removeFromLeft(UILayout::panelMargin);

            devicePanels[devicePanelIndex++]->setBounds(bounds);

            left = bounds.getRight() + UILayout::panelMargin;
        }

        top = juce::roundToInt(rowBounds.getBottom()) + UILayout::panelMargin;
    }
}

void DevicePanelContainer::connectToDevice(const ximu3::ConnectionInfo& connectionInfo)
{
    auto connection = std::make_shared<ximu3::Connection>(connectionInfo);
    connection->openAsync([&, connection](auto result)
                          {
                              juce::MessageManager::callAsync([&, connection, result]
                                                              {
                                                                  if (result != ximu3::XIMU3_ResultOk)
                                                                  {
                                                                      DialogQueue::getSingleton().pushBack(std::make_unique<ErrorDialog>("Unable to open connection " + connection->getInfo()->toString() + "."));
                                                                      return;
                                                                  }

                                                                  addAndMakeVisible(*devicePanels.emplace_back(std::make_unique<DevicePanel>(windowLayout, connection, glRenderer, *this, [&]
                                                                  {
                                                                      static unsigned int counter;

                                                                          if (devicePanels.empty() || (++counter >= UIColours::tags.size()))
                                                                          {
                                                                              counter = 0;
                                                                          }

                                                                      return UIColours::tags[counter];
                                                                  }())));

                                                                  devicePanelsSizeChanged();
                                                              });
                          });
}

std::vector<DevicePanel*> DevicePanelContainer::getDevicePanels() const
{
    std::vector<DevicePanel*> vector;
    for (auto& devicePanel : devicePanels)
    {
        vector.push_back(devicePanel.get());
    }
    return vector;
}

void DevicePanelContainer::removeAllPanels()
{
    devicePanels.clear();
    devicePanelsSizeChanged();
}

void DevicePanelContainer::removePanel(const DevicePanel& panel)
{
    for (size_t index = 0; index < devicePanels.size(); index++)
    {
        if (devicePanels[index].get() == &panel)
        {
            devicePanels.erase(devicePanels.begin() + (int) index);
            break;
        }
    }
    devicePanelsSizeChanged();
}

void DevicePanelContainer::movePanel(DevicePanel& move, DevicePanel& target)
{
    for (size_t index = 0; index < devicePanels.size(); index++)
    {
        if (devicePanels[index].get() == &move)
        {
            auto moving = std::move(devicePanels[index]);
            for (; index < devicePanels.size() - 1; index++)
            {
                devicePanels[index] = std::move(devicePanels[index + 1]);
                if (devicePanels[index].get() == &target)
                {
                    devicePanels[index + 1] = std::move(moving);
                    resized();
                    return;
                }
            }
        }
        if (devicePanels[index].get() == &target)
        {
            auto& targetPointer = devicePanels[index++];
            for (; index < devicePanels.size(); index++)
            {
                std::swap(devicePanels[index], targetPointer);
                if (targetPointer.get() == &move)
                {
                    resized();
                    return;
                }
            }
        }
    }
}

void DevicePanelContainer::showDragOverlayAtComponent(juce::Component& component, DragOverlay::Side side)
{
    if (dragOverlay == nullptr || dragOverlay->getTarget() != &component)
    {
        dragOverlay.reset();
        dragOverlay = std::make_unique<DragOverlay>(component);
        component.addAndMakeVisible(*dragOverlay);
    }

    dragOverlay->setSide(side);
}

void DevicePanelContainer::hideDragOverlay()
{
    dragOverlay.reset();
}

DragOverlay* DevicePanelContainer::getCurrentlyShowingDragOverlay()
{
    return dragOverlay.get();
}

void DevicePanelContainer::setLayout(Layout layout_)
{
    if (layout == layout_)
    {
        return;
    }

    layout = layout_;

    accordionResizeBar.setVisible(layout == Layout::accordion);
    setExpandedDevicePanel(expandedDevicePanel);

    setBounds(findParentComponentOfClass<juce::Viewport>()->getBounds());
    resized();
}

DevicePanelContainer::Layout DevicePanelContainer::getLayout()
{
    return layout;
}

void DevicePanelContainer::updateHeightInAccordionMode()
{
    if (layout != Layout::accordion)
    {
        return;
    }

    int height = 0;
    for (size_t index = 0; index < devicePanels.size(); index++)
    {
        height += UILayout::panelMargin + (expandedDevicePanel == devicePanels[index].get() ? expandedPanelHeight : DevicePanel::collapsedHeight);
    }

    if (getHeight() == height)
    {
        resized();
        return;
    }
    setSize(getWidth(), height);
}

void DevicePanelContainer::setExpandedDevicePanel(DevicePanel* const devicePanel)
{
    expandedDevicePanel = devicePanel;

    if (expandedDevicePanel == nullptr || layout != Layout::accordion)
    {
        for (auto& panel : devicePanels)
        {
            panel->setAlpha(1.0f);
        }
    }
    else
    {
        for (auto& panel : devicePanels)
        {
            panel->setAlpha(0.5f);
        }
        expandedDevicePanel->setAlpha(1.0f);
    }

    updateHeightInAccordionMode();
}

const DevicePanel* DevicePanelContainer::getExpandedDevicePanel() const
{
    return expandedDevicePanel;
}

DevicePanelContainer::AccordionResizeBar::AccordionResizeBar()
{
    setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
}

void DevicePanelContainer::AccordionResizeBar::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    auto* const devicePanelContainer = static_cast<DevicePanelContainer*>(getParentComponent());
    const auto newHeight = mouseEvent.getEventRelativeTo(devicePanelContainer->expandedDevicePanel).getPosition().getY() - getHeight() / 2;
    devicePanelContainer->expandedPanelHeight = juce::jmax(DevicePanel::collapsedHeight, newHeight);
    devicePanelContainer->updateHeightInAccordionMode();
}

void DevicePanelContainer::devicePanelsSizeChanged()
{
    setExpandedDevicePanel(expandedDevicePanel);
    resized();
    onDevicePanelsSizeChanged();
}
