#pragma once

#include <algorithm>
#include "OpenGLResources.h"
#include "OpenGL/OpenGLComponent.h"
#include <vector>

class OpenGLRenderer : private juce::OpenGLRenderer
{
public:
    OpenGLRenderer(juce::Component& attachTo, juce::ThreadPool& threadPool_);

    ~OpenGLRenderer() override;

    juce::OpenGLContext& getContext();

    void addComponent(OpenGLComponent& component);

    void removeComponent(OpenGLComponent& component);

    OpenGLResources& getResources();

private:
    juce::ThreadPool& threadPool;

    juce::OpenGLContext context;

    // Synchronize access to data shared between the JUCE Message thread and the OpenGL thread such as the components list.
    std::mutex sharedGLDataLock;

    std::vector<OpenGLComponent*> components;

    std::unique_ptr<OpenGLResources> resources;

    // Reset OpenGL state to the default settings expected by our OpenGLComponents.
    static void resetDefaultOpenGLState();

    void newOpenGLContextCreated() override;

    void renderOpenGL() override;

    void openGLContextClosing() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLRenderer)
};
