#pragma once

#include "CustomLookAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>

class DragOverlay : public juce::Component,
                    private juce::ComponentListener
{
public:
    explicit DragOverlay(juce::Component& target_) : target(&target_)
    {
        target->addComponentListener(this);
        setAlwaysOnTop(true);
        updateBoundsToTarget();
    }

    ~DragOverlay() override
    {
        if (target != nullptr)
        {
            target->removeComponentListener(this);
        }
    }

    void paint(juce::Graphics& g) override
    {
        const auto overlayBounds = [&]
        {
            const auto size = (side == Side::bottom || side == Side::top) ? getHeight() / 2 : getWidth() / 2;

            switch (side)
            {
                case Side::top:
                    return getLocalBounds().removeFromTop(size);
                case Side::right:
                    return getLocalBounds().removeFromRight(size);
                case Side::bottom:
                    return getLocalBounds().removeFromBottom(size);
                case Side::left:
                    return getLocalBounds().removeFromLeft(size);
                case Side::all:
                    return getLocalBounds();
                default:
                    return juce::Rectangle<int>();
            }
        }();

        g.setColour(UIColours::foreground);
        g.drawRect(overlayBounds, 3);
    }

    juce::Component* getTarget()
    {
        return target;
    }

    enum class Side
    {
        top,
        right,
        bottom,
        left,
        all,
    };

    void setSide(Side side_)
    {
        if (side != side_)
        {
            side = side_;
            repaint();
        }
    }

    Side getSide() const
    {
        return side;
    }

private:
    juce::Component::SafePointer<juce::Component> target;
    Side side = Side::top;

    void componentMovedOrResized(juce::Component&, bool, bool) override
    {
        updateBoundsToTarget();
    }

    void updateBoundsToTarget()
    {
        if (target != nullptr)
        {
            setBounds(target->getLocalBounds());
        }
        else
        {
            jassertfalse;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DragOverlay)
};
