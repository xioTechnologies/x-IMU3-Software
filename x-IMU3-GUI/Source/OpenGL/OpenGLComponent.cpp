#include "OpenGLComponent.h"

OpenGLComponent::OpenGLComponent(juce::OpenGLContext& context_) : juce::ComponentMovementWatcher(this), context(context_)
{
    // Only visualisation, no mouse interaction
    setInterceptsMouseClicks(false, false);
}

juce::Rectangle<int> OpenGLComponent::getBoundsInMainWindow() const
{
    std::lock_guard _(boundsInMainWindowLock);
    return boundsInMainWindow;
}

juce::Rectangle<int> OpenGLComponent::toOpenGLBounds(const juce::Rectangle<int>& bounds) const
{
    const auto desktopScale = context.getRenderingScale();
    return juce::Rectangle<decltype(desktopScale)>(bounds.getX() * desktopScale,
                                                   (topLevelHeight - bounds.getBottom()) * desktopScale,
                                                   bounds.getWidth() * desktopScale,
                                                   bounds.getHeight() * desktopScale).toNearestInt();
}

void OpenGLComponent::componentMovedOrResized(bool, bool)
{
    topLevelHeight = getTopLevelComponent()->getHeight();

    std::lock_guard _(boundsInMainWindowLock);
    boundsInMainWindow = getTopLevelComponent()->getLocalArea(this, getLocalBounds());
}
