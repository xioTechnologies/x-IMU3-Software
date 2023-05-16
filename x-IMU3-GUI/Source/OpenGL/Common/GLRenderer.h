#pragma once

#include "../OpenGLComponent.h"
#include <algorithm>
#include "GLResources.h"
#include <vector>

class GLRenderer : private juce::OpenGLRenderer
{
public:
    explicit GLRenderer(juce::Component& attachTo);

    ~GLRenderer() override;

    juce::OpenGLContext& getContext();

    void addComponent(OpenGLComponent& component);

    void removeComponent(OpenGLComponent& component);

    GLResources& getResources();

    void refreshScreen(const juce::Colour& colour, const juce::Rectangle<GLint>& viewport);

private:
    juce::OpenGLContext context;

    // Synchronize access to data shared between the JUCE Message thread and the OpenGL thread such as the components list.
    std::mutex sharedGLDataLock;

    std::vector<OpenGLComponent*> components;

    std::unique_ptr<GLResources> resources;

    void newOpenGLContextCreated() override;

    void renderOpenGL() override;

    void openGLContextClosing() override;
};
