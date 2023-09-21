#pragma once

#include "Common/GLResources.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_opengl/juce_opengl.h>
#include <mutex>

class OpenGLComponent : public juce::Component,
                        private juce::ComponentListener
{
public:
    explicit OpenGLComponent(juce::OpenGLContext& context_);

    ~OpenGLComponent() override;

    void parentHierarchyChanged() override;

    virtual void render() = 0;

protected:
    juce::OpenGLContext& context;

    GLResources* resources = nullptr;

    juce::Rectangle<int> getBoundsInMainWindow() const;

    juce::Rectangle<int> toOpenGLBounds(const juce::Rectangle<int>& bounds) const;

private:
    std::vector<juce::Component*> registeredParents;

    std::atomic<int> topLevelHeight = 0;

    mutable std::mutex boundsInMainWindowLock;
    juce::Rectangle<int> boundsInMainWindow;

    void unregisterParents();

    void updateBounds();

    void componentMovedOrResized(juce::Component&, bool, bool) override;

    friend class GLRenderer; // sets resources

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLComponent)
};
