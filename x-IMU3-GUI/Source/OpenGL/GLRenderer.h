#pragma once

#include "GLResources.h"
#include "OpenGLComponent.h"

class GLRenderer : private juce::OpenGLRenderer
{
public:
    explicit GLRenderer(juce::Component& attachTo);

    ~GLRenderer() override;

    juce::OpenGLContext& getContext();

    void addComponent(OpenGLComponent& component);

    void removeComponent(const OpenGLComponent& component);

    GLResources& getResources();

    void turnCullingOff();

    void turnDepthTestOff();

    void refreshScreen(const juce::Colour& colour, const juce::Rectangle<GLint>& viewport);

    const juce::Matrix3D<GLfloat>& getProjectionMatrix(juce::Rectangle<GLint> viewport);

private:
    juce::OpenGLContext context;

    std::mutex componentsLock;
    std::vector<OpenGLComponent*> components;

    std::unique_ptr<GLResources> resources;

    juce::Matrix3D<GLfloat> viewMatrix;
    juce::Matrix3D<GLfloat> projectionMatrix;

    void newOpenGLContextCreated() override;

    void renderOpenGL() override;

    void openGLContextClosing() override;
};
