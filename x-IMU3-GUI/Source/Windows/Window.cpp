#include "DevicePanel/DevicePanel.h"
#include "DevicePanelContainer.h"
#include "Window.h"
#include "WindowIDs.h"

Window::Title::Title(Window& parentWindow_) : SimpleLabel(parentWindow_.getName(), UIFonts::smallFont, juce::Justification::centred), parentWindow(parentWindow_)
{
    setInterceptsMouseClicks(true, true);
    setMouseCursor({ juce::MouseCursor::DraggingHandCursor });
}

void Window::Title::mouseDown(const juce::MouseEvent& mouseEvent)
{
    mouseDrag(mouseEvent);
}

void Window::Title::mouseDrag(const juce::MouseEvent& mouseEvent)
{
    if (auto* const component = parentWindow.devicePanel.getComponentAt(mouseEvent.getScreenPosition() - parentWindow.devicePanel.getScreenPosition()))
    {
        if (auto* const window = [&]
        {
            if (auto* const casted = dynamic_cast<Window*>(component))
            {
                return casted;
            }

            return component->findParentComponentOfClass<Window>();
        }())
        {
            parentWindow.devicePanel.getDevicePanelContainer().showDragOverlayAtComponent(*window, [&]
            {
                if (window == &parentWindow)
                {
                    return DragOverlay::Side::all;
                }

                const auto positionRelative = mouseEvent.getScreenPosition() - window->getScreenPosition();
                const auto proportionToTop = positionRelative.y / (float) window->getHeight();
                const auto proportionToRight = 1.0f - positionRelative.x / (float) window->getWidth();
                const auto proportionToBottom = 1.0f - positionRelative.y / (float) window->getHeight();
                const auto proportionToLeft = positionRelative.x / (float) window->getWidth();

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
            parentWindow.devicePanel.getDevicePanelContainer().hideDragOverlay();
        }
    }
}

void Window::Title::mouseUp(const juce::MouseEvent& mouseEvent)
{
    mouseDrag(mouseEvent);

    auto* const overlay = parentWindow.devicePanel.getDevicePanelContainer().getCurrentlyShowingDragOverlay();
    const auto* const window = overlay ? dynamic_cast<Window*>(overlay->getTarget()) : nullptr;
    if (window != nullptr && window != &parentWindow)
    {
        auto movingTree = findWindow(parentWindow.windowLayout, parentWindow.type);
        removeFromParent(movingTree);

        const auto targetTree = findWindow(parentWindow.windowLayout, window->type);
        movingTree.setProperty(WindowIDs::size, targetTree.getProperty(WindowIDs::size, 1.0f), nullptr);

        const auto side = parentWindow.devicePanel.getDevicePanelContainer().getCurrentlyShowingDragOverlay()->getSide();
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

    parentWindow.devicePanel.getDevicePanelContainer().hideDragOverlay();
}

Window::Window(const juce::ValueTree& windowLayout_, const juce::Identifier& type_, DevicePanel& devicePanel_)
        : juce::Component(getWindowTitle(type_)),
          devicePanel(devicePanel_),
          windowLayout(windowLayout_),
          type(type_)
{
    addAndMakeVisible(title);
}

void Window::paint(juce::Graphics& g)
{
    g.setColour(findColour(juce::ResizableWindow::backgroundColourId));
    g.fillRect(title.getBounds());
}

void Window::resized()
{
    title.setBounds(getLocalBounds().removeFromTop(22));
}

juce::Rectangle<int> Window::getContentBounds() const
{
    return getLocalBounds().withTrimmedTop(title.getHeight());
}

const juce::Identifier& Window::getType() const
{
    return type;
}

juce::ValueTree Window::findWindow(const juce::ValueTree root, const juce::Identifier& type)
{
    for (auto child : root)
    {
        if (child.hasType(type))
        {
            return child;
        }

        auto childOfChild = findWindow(child, type);
        if (childOfChild.isValid())
        {
            return childOfChild;
        }
    }
    return {};
}

void Window::removeFromParent(juce::ValueTree tree)
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
