#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_opengl/juce_opengl.h>
#include <mutex>

class OpenGLComponent : public juce::Component,
                        private juce::ComponentMovementWatcher
{
public:
    explicit OpenGLComponent(juce::OpenGLContext& context_);

    virtual void render() = 0;

protected:
    juce::OpenGLContext& context;

    juce::Rectangle<int> getBoundsInMainWindow() const;

    juce::Rectangle<int> toOpenGLBounds(const juce::Rectangle<int>& bounds) const;

private:
    std::atomic<int> topLevelHeight = 0;

    mutable std::mutex boundsInMainWindowLock;
    juce::Rectangle<int> boundsInMainWindow;

    void componentMovedOrResized(bool, bool) override;

    void componentPeerChanged() override
    {
    }

    void componentVisibilityChanged() override
    {
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLComponent)
};
