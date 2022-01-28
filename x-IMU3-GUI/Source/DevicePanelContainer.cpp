#include "ApplicationSettings.h"
#include "DevicePanel/DevicePanel.h"
#include "DevicePanelContainer.h"

DevicePanelContainer::DevicePanelContainer(juce::ValueTree& windowLayout_, GLRenderer& glRenderer_)
        : windowLayout(windowLayout_),
          glRenderer(glRenderer_)
{
    // Make sure dispatcher singleton exist
    NetworkDiscoveryDispatcher::getSingleton();

    addChildComponent(&accordionResizeBar);
}

DevicePanelContainer::~DevicePanelContainer()
{
}

void DevicePanelContainer::resized()
{
    if (devicePanels.empty())
    {
        return;
    }

    if (layout == Layout::accordion)
    {
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

    int numberOfRows, numberOfColumns;

    if (layout == Layout::rows)
    {
        numberOfRows = (int) devicePanels.size();
        numberOfColumns = 1;
    }
    else if (layout == Layout::columns)
    {
        numberOfRows = 1;
        numberOfColumns = (int) devicePanels.size();
    }
    else
    {
        const auto squareRoot = std::sqrt(devicePanels.size());
        numberOfRows = juce::roundToInt(squareRoot);
        numberOfColumns = (int) std::ceil(squareRoot);
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

        top = devicePanels[devicePanelIndex - 1]->getBottom() + UILayout::panelMargin;
    }
}

void DevicePanelContainer::connectToDevice(const ximu3::ConnectionInfo& connectionInfo)
{
    auto connection = std::make_shared<ximu3::Connection>(connectionInfo);
    connection->openAsync([&, connection](auto result)
                          {
                              if (result != ximu3::XIMU3_ResultOk)
                              {
                                  ApplicationErrorsDialog::addError("Unable to open connection " + connection->getInfo()->toString() + ".");
                                  return;
                              }

                              juce::MessageManager::callAsync([&, connection]
                                                              {
                                                                  onDevicePanelsSizeChanged((int) devicePanels.size(), (int) devicePanels.size() + 1);

                                                                  addAndMakeVisible(*devicePanels.emplace_back(std::make_unique<DevicePanel>(windowLayout, connection, glRenderer, *this, [&]
                                                                  {
                                                                      static unsigned int counter;

                                                                      if (devicePanels.empty() || (++counter >= UIColours::panelTags.size()))
                                                                      {
                                                                          counter = 0;
                                                                      }

                                                                      return UIColours::panelTags[counter];
                                                                  }())));

                                                                  toggleAccordionState(nullptr);

                                                                  if (layout != Layout::accordion)
                                                                  {
                                                                      resized();
                                                                  }
                                                              });
                          });
}

std::vector<std::unique_ptr<DevicePanel>>& DevicePanelContainer::getDevicePanels()
{
    return devicePanels;
}

void DevicePanelContainer::removeAllPanels()
{
    onDevicePanelsSizeChanged((int) devicePanels.size(), 0);
    devicePanels.clear();
    resized();
}

void DevicePanelContainer::removePanel(DevicePanel& panel)
{
    onDevicePanelsSizeChanged((int) devicePanels.size(), (int) devicePanels.size() - 1);
    for (size_t index = 0; index < devicePanels.size(); index++)
    {
        if (devicePanels[index].get() == &panel)
        {
            devicePanels.erase(devicePanels.begin() + (int) index);
            break;
        }
    }
    resized();
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
    toggleAccordionState(nullptr);

    setBounds(getParentComponent()->getBounds());
    resized();
}

DevicePanelContainer::Layout DevicePanelContainer::getLayout()
{
    return layout;
}

void DevicePanelContainer::sendCommands(const std::vector<CommandMessage>& commandMessages)
{
    for (auto& devicePanel : devicePanels)
    {
        devicePanel->sendCommands(commandMessages);
    }
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

void DevicePanelContainer::toggleAccordionState(DevicePanel* const devicePanel)
{
    if (layout == Layout::accordion && expandedDevicePanel != devicePanel)
    {
        expandedDevicePanel = devicePanel;
    }
    else
    {
        expandedDevicePanel = nullptr;
    }

    if (expandedDevicePanel == nullptr)
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
