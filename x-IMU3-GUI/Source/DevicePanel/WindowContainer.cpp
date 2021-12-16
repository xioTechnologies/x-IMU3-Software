#include "CustomLookAndFeel.h"
#include "DevicePanel/DevicePanel.h"
#include <numeric>
#include "WindowContainer.h"
#include "Windows/Window.h"

class WindowContainer::ResizeBar : public juce::Component
{
public:
    ResizeBar(WindowContainer& parent_, size_t index_)
            : parent(parent_),
              index(index_)
    {
        setMouseCursor({ parent.isVertical ? juce::MouseCursor::UpDownResizeCursor : juce::MouseCursor::LeftRightResizeCursor });
    }

    void mouseDrag(const juce::MouseEvent& mouseEvent)
    {
        const auto desiredPosition = parent.isVertical ? (getBounds().getCentreY() + mouseEvent.getMouseDownY() + mouseEvent.getDistanceFromDragStartY())
                                                       : (getBounds().getCentreX() + mouseEvent.getMouseDownX() + mouseEvent.getDistanceFromDragStartX());

        auto& prevComponent = *parent.childComponents[index];
        auto& nextComponent = *parent.childComponents[index + 1];

        auto offset = desiredPosition - (parent.isVertical ? getBounds().getCentreY() : getBounds().getCentreX());

        static constexpr int minimumComponentSize = 50;
        if ((offset < 0 && (offset < -(parent.isVertical ? prevComponent.getHeight() : prevComponent.getWidth()) + minimumComponentSize)) ||
            (offset > 0 && (offset > (parent.isVertical ? nextComponent.getHeight() : nextComponent.getWidth()) - minimumComponentSize)))
        {
            return;
        }

        if (parent.isVertical)
        {
            prevComponent.setBounds(prevComponent.getBounds().withBottom(prevComponent.getBottom() + offset));
            setBounds(getBounds().translated(0, offset));
            nextComponent.setBounds(nextComponent.getBounds().withTop(nextComponent.getBounds().getY() + offset));
        }
        else
        {
            prevComponent.setBounds(prevComponent.getBounds().withRight(prevComponent.getRight() + offset));
            setBounds(getBounds().translated(offset, 0));
            nextComponent.setBounds(nextComponent.getBounds().withLeft(nextComponent.getBounds().getX() + offset));
        }
    }

    void mouseUp(const juce::MouseEvent&)
    {
        updateTreeState();
    }

private:
    WindowContainer& parent;
    const size_t index;

    void updateTreeState()
    {
        for (size_t i = 0; i < parent.childComponents.size(); i++)
        {
            const auto componentSize = parent.isVertical ? parent.childComponents[i]->getHeight() : parent.childComponents[i]->getWidth();
            const auto sizeProportion = componentSize / (float) parent.totalWindowSize;
            parent.layoutTree.getChild((int) i).setPropertyExcludingListener(&parent, WindowIDs::size, sizeProportion, nullptr);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ResizeBar)
};

WindowContainer::WindowContainer(DevicePanel& devicePanel_, juce::ValueTree layoutTree_)
        : devicePanel(devicePanel_),
          layoutTree(layoutTree_)
{
    layoutTree.addListener(this);

    for (auto item : layoutTree)
    {
        valueTreeChildAdded(layoutTree, item);
    }
}

WindowContainer::~WindowContainer()
{
}

void WindowContainer::resized()
{
    if (childComponents.empty())
    {
        return;
    }

    totalWindowSize = juce::jmax(1, (isVertical ? getHeight() : getWidth()) - (int) (UILayout::panelMargin * resizeBars.size()));

    const auto sizeProportionsSum = std::accumulate(layoutTree.begin(), layoutTree.end(), 0.0f, [](float sum, auto child)
    {
        return sum + (float) child.getProperty(WindowIDs::size, 1.0f);
    });

    int position = 0;
    for (int i = 0; i < (int) (childComponents.size() + resizeBars.size()) - 1; i++)
    {
        if (i % 2 == 0)
        {
            const auto index = i / 2;
            const auto size = juce::roundToInt(totalWindowSize * (float) layoutTree.getChild(index).getProperty(WindowIDs::size, 1.0f) / sizeProportionsSum);
            if (isVertical)
            {
                childComponents[(size_t) index]->setBounds(0, position, getWidth(), size);
            }
            else
            {
                childComponents[(size_t) index]->setBounds(position, 0, size, getHeight());
            }
            position += size;
        }
        else
        {
            const auto index = (i - 1) / 2;
            if (isVertical)
            {
                resizeBars[(size_t) index]->setBounds(0, position, getWidth(), UILayout::panelMargin);
            }
            else
            {
                resizeBars[(size_t) index]->setBounds(position, 0, UILayout::panelMargin, getHeight());
            }
            position += UILayout::panelMargin;
        }
    }
    if (isVertical)
    {
        childComponents.back()->setBounds(getLocalBounds().withTop(position));
    }
    else
    {
        childComponents.back()->setBounds(getLocalBounds().withLeft(position));
    }
}

void WindowContainer::componentAddedOrRemoved()
{
    auto numberOfResizeBars = resizeBars.size();
    resizeBars.resize(juce::jmax<size_t>(1, childComponents.size()) - 1);
    while (numberOfResizeBars < resizeBars.size())
    {
        resizeBars[numberOfResizeBars] = std::make_unique<ResizeBar>(*this, numberOfResizeBars);
        addAndMakeVisible(*resizeBars[numberOfResizeBars++]);
    }
    resized();
}

void WindowContainer::valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property)
{
    if (property == WindowIDs::size && tree.getParent() == layoutTree)
    {
        resized();
    }
}

void WindowContainer::valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded)
{
    if (parentTree != layoutTree)
    {
        return;
    }

    std::shared_ptr<juce::Component> componentToAdd;
    if (childWhichHasBeenAdded.hasType(WindowIDs::Row) || childWhichHasBeenAdded.hasType(WindowIDs::Column))
    {
        componentToAdd = std::make_shared<WindowContainer>(devicePanel, childWhichHasBeenAdded);
    }
    else if (!(componentToAdd = devicePanel.getOrCreateWindow(childWhichHasBeenAdded)))
    {
        return;
    }
    addAndMakeVisible(*componentToAdd);
    childComponents.insert(childComponents.begin() + parentTree.indexOf(childWhichHasBeenAdded), componentToAdd);

    componentAddedOrRemoved();
}

void WindowContainer::valueTreeChildRemoved(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenRemoved, int)
{
    if (parentTree != layoutTree)
    {
        return;
    }

    juce::Component* componentToRemove = nullptr;

    for (auto& component : childComponents)
    {
        if (auto* windowLayoutComponent = dynamic_cast<WindowContainer*>(component.get()))
        {
            if (windowLayoutComponent->layoutTree == childWhichHasBeenRemoved)
            {
                componentToRemove = component.get();
                break;
            }
        }
        else if (auto* devicePanelWindow = dynamic_cast<Window*>(component.get()))
        {
            if (devicePanelWindow->getType() == childWhichHasBeenRemoved.getType())
            {
                componentToRemove = component.get();
                break;
            }
        }
    }

    if (componentToRemove == nullptr)
    {
        jassertfalse; // this assert indicates that the removed child was not found
    }

    removeChildComponent(componentToRemove);

    devicePanel.cleanupWindows();

    for (int i = 0; i < (int) childComponents.size(); i++)
    {
        if (childComponents[(size_t) i].get() == componentToRemove)
        {
            childComponents.erase(childComponents.begin() + i);
            break;
        }
    }

    componentAddedOrRemoved();
}

void WindowContainer::valueTreeChildOrderChanged(juce::ValueTree&, int, int)
{
    jassertfalse;
}
