#include "DevicePanel/DevicePanel.h"
#include "DevicePanelContainer.h"
#include "DragOverlay.h"
#include "WindowHeader.h"
#include "WindowIDs.h"

WindowHeader::WindowHeader(DevicePanel& devicePanel_, const juce::ValueTree& windowLayout_, const juce::Identifier& type_, const juce::String& menuButtonTooltip, std::function<juce::PopupMenu()> getPopup)
        : devicePanel(devicePanel_),
          windowLayout(windowLayout_),
          type(type_),
          menuButton(BinaryData::menu_svg, menuButtonTooltip, getPopup, false),
          title(getWindowTitle(type_), UIFonts::getSmallFont(), juce::Justification::centred)
{
    addAndMakeVisible(menuButton);
    addAndMakeVisible(title);

    setInterceptsMouseClicks(true, true);
    setMouseCursor({ juce::MouseCursor::DraggingHandCursor });
}

void WindowHeader::paint(juce::Graphics& g)
{
    g.setColour(findColour(juce::ResizableWindow::backgroundColourId));
    g.fillRect(getLocalBounds());
}

void WindowHeader::resized()
{
    auto bounds = getLocalBounds().reduced(2);
    menuButton.setBounds(bounds.removeFromLeft(bounds.getHeight()));
    title.setBounds(bounds);
}

void WindowHeader::mouseDown(const juce::MouseEvent& mouseEvent)
{
    mouseDrag(mouseEvent);
}

void WindowHeader::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    if (auto* const component = devicePanel.getComponentAt(mouseEvent.getScreenPosition() - devicePanel.getScreenPosition()))
    {
        if (auto* const targetWindow = [&]
        {
            if (auto* const casted = dynamic_cast<Window*>(component))
            {
                return casted;
            }

            return component->findParentComponentOfClass<Window>();
        }())
        {
            devicePanel.getDevicePanelContainer().showDragOverlayAtComponent(*targetWindow, [&]
            {
                if (targetWindow == getParentComponent())
                {
                    return DragOverlay::Side::all;
                }

                const auto positionRelative = mouseEvent.getScreenPosition() - targetWindow->getScreenPosition();
                const auto proportionToTop = positionRelative.y / (float) targetWindow->getHeight();
                const auto proportionToRight = 1.0f - positionRelative.x / (float) targetWindow->getWidth();
                const auto proportionToBottom = 1.0f - positionRelative.y / (float) targetWindow->getHeight();
                const auto proportionToLeft = positionRelative.x / (float) targetWindow->getWidth();

                auto min = juce::jmin(proportionToTop, proportionToRight, proportionToBottom, proportionToLeft);
                if (proportionToTop == min)
                {
                    return DragOverlay::Side::top;
                }
                if (proportionToRight == min)
                {
                    return DragOverlay::Side::right;
                }
                if (proportionToBottom == min)
                {
                    return DragOverlay::Side::bottom;
                }
                return DragOverlay::Side::left;
            }());
        }
        else
        {
            devicePanel.getDevicePanelContainer().hideDragOverlay();
        }
    }
}

void WindowHeader::mouseUp(const juce::MouseEvent& mouseEvent)
{
    mouseDrag(mouseEvent);

    auto* const overlay = devicePanel.getDevicePanelContainer().getCurrentlyShowingDragOverlay();
    const auto* const targetWindow = overlay ? dynamic_cast<Window*>(overlay->getTarget()) : nullptr;
    if (targetWindow != nullptr && targetWindow != getParentComponent())
    {
        auto movingTree = findWindow(windowLayout, type);
        removeFromParent(movingTree);

        const auto targetTree = findWindow(windowLayout, targetWindow->getType());
        movingTree.setProperty(WindowIDs::size, targetTree.getProperty(WindowIDs::size, 1.0f), nullptr);

        const auto side = devicePanel.getDevicePanelContainer().getCurrentlyShowingDragOverlay()->getSide();
        const auto add = (side == DragOverlay::Side::bottom || side == DragOverlay::Side::right) ? 1 : 0;

        auto parent = targetTree.getParent();
        if (parent.hasType(WindowIDs::Column) == (side == DragOverlay::Side::top || side == DragOverlay::Side::bottom))
        {
            parent.addChild(movingTree, parent.indexOf(targetTree) + add, nullptr);
        }
        else
        {
            const auto containerIndex = parent.indexOf(targetTree);
            parent.removeChild(targetTree, nullptr);

            juce::ValueTree container(parent.getType() == WindowIDs::Column ? WindowIDs::Row : WindowIDs::Column);
            container.setProperty(WindowIDs::size, targetTree.getProperty(WindowIDs::size, 1.0f), nullptr);
            container.addChild(targetTree, 0, nullptr);
            container.addChild(movingTree, add, nullptr);
            parent.addChild(container, containerIndex, nullptr);
        }
    }

    devicePanel.getDevicePanelContainer().hideDragOverlay();
}

void WindowHeader::removeFromParent(juce::ValueTree tree)
{
    auto parent = tree.getParent();
    parent.removeChild(tree, nullptr);

    // If the parent is left with no children, destroy the parent
    if (parent.getNumChildren() == 0 && parent.isValid())
    {
        removeFromParent(parent);
        return;
    }

    // If the parent is left with a single child, move that child and destroy parent
    if (parent.getNumChildren() == 1 && parent.getParent().isValid())
    {
        auto child = parent.getChild(0);

        const auto insertIndex = parent.getParent().indexOf(parent);

        parent = parent.getParent(); // reassign parent to its parent
        parent.removeChild(insertIndex, nullptr);

        // If the child is a window, move it
        if (child.getNumChildren() == 0)
        {
            child.getParent().removeChild(child, nullptr);
            parent.addChild(child, insertIndex, nullptr);
            return;
        }

        // If the child is a container, move all children
        while (child.getNumChildren() > 0)
        {
            const auto move = child.getChild(child.getNumChildren() - 1);
            child.removeChild(move, nullptr);
            parent.addChild(move, insertIndex, nullptr);
        }
    }
}
