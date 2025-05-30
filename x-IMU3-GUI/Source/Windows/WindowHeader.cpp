#include "ConnectionPanel/ConnectionPanel.h"
#include "ConnectionPanelContainer.h"
#include "Widgets/DragOverlay.h"
#include "WindowHeader.h"
#include "WindowIds.h"

WindowHeader::WindowHeader(ConnectionPanel& connectionPanel_, const juce::ValueTree& windowLayout_, const juce::Identifier& type_, const juce::String& menuButtonTooltip, std::function<juce::PopupMenu()> getPopup)
    : connectionPanel(connectionPanel_),
      windowLayout(windowLayout_),
      type(type_),
      menuButton(BinaryData::menu_svg, menuButtonTooltip, getPopup, false),
      title(windowTitles.at(type_), UIFonts::getSmallFont(), juce::Justification::centred)
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
    if (auto* const component = connectionPanel.getComponentAt(mouseEvent.getScreenPosition() - connectionPanel.getScreenPosition()))
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
            connectionPanel.getConnectionPanelContainer().showDragOverlayAtComponent(*targetWindow, [&]
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

                const auto min = juce::jmin(proportionToTop, proportionToRight, proportionToBottom, proportionToLeft);
                if (juce::exactlyEqual(proportionToTop, min))
                {
                    return DragOverlay::Side::top;
                }
                if (juce::exactlyEqual(proportionToRight, min))
                {
                    return DragOverlay::Side::right;
                }
                if (juce::exactlyEqual(proportionToBottom, min))
                {
                    return DragOverlay::Side::bottom;
                }
                return DragOverlay::Side::left;
            }());
        }
        else
        {
            connectionPanel.getConnectionPanelContainer().hideDragOverlay();
        }
    }
}

void WindowHeader::mouseUp(const juce::MouseEvent& mouseEvent)
{
    mouseDrag(mouseEvent);

    auto* const overlay = connectionPanel.getConnectionPanelContainer().getCurrentlyShowingDragOverlay();
    const auto* const targetWindow = overlay ? dynamic_cast<Window*>(overlay->getTarget()) : nullptr;
    if (targetWindow != nullptr && targetWindow != getParentComponent())
    {
        auto movingTree = findWindow(windowLayout, type);
        removeFromParent(movingTree);

        const auto targetTree = findWindow(windowLayout, targetWindow->getType());
        movingTree.setProperty(WindowIds::size, targetTree.getProperty(WindowIds::size, 1.0f), nullptr);

        const auto side = connectionPanel.getConnectionPanelContainer().getCurrentlyShowingDragOverlay()->getSide();
        const auto add = (side == DragOverlay::Side::bottom || side == DragOverlay::Side::right) ? 1 : 0;

        auto parent = targetTree.getParent();
        if (parent.hasType(WindowIds::Column) == (side == DragOverlay::Side::top || side == DragOverlay::Side::bottom))
        {
            parent.addChild(movingTree, parent.indexOf(targetTree) + add, nullptr);
        }
        else
        {
            const auto containerIndex = parent.indexOf(targetTree);
            parent.removeChild(targetTree, nullptr);

            juce::ValueTree container(parent.getType() == WindowIds::Column ? WindowIds::Row : WindowIds::Column);
            container.setProperty(WindowIds::size, targetTree.getProperty(WindowIds::size, 1.0f), nullptr);
            container.addChild(targetTree, 0, nullptr);
            container.addChild(movingTree, add, nullptr);
            parent.addChild(container, containerIndex, nullptr);
        }
    }

    connectionPanel.getConnectionPanelContainer().hideDragOverlay();
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
